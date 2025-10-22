/**
 * \file ar_osal_serverg.c
 *
 * \brief
 *       This file has implementation of service location, notification, and
 *       state registration.
 *
 * \copyright
 *  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *  SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#define AR_OSAL_SERVREG_TAG    "COSR"

#include "ar_osal_types.h"
#include "ar_osal_servreg.h"
#include "ar_osal_mutex.h"
#include "ar_osal_error.h"
#include "ar_osal_log.h"
#include "ar_osal_mem_op.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef AR_OSAL_USE_CUTILS
#include <cutils/properties.h>
#endif

 #ifdef __cplusplus
 extern "C" {
 #endif /* __cplusplus */

#ifdef AR_OSAL_USE_PD_NOTIFIER
#include <libpdmapper.h>
#include <libpdnotifier.h>
#include <service_registry_notifier_v01.h>
SR_DL_Handle*       pd_mapper_handle;
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/poll.h>
#include "ar_osal_sys_id.h"
char_t domain_name[AR_SUB_SYS_ID_LAST + 1][24] = { "msm/adsp/audio_pd", "msm/mdsp/audio_pd", "msm/adsp/audio_pd","msm/apss/audio_pd","msm/sdsp/audio_pd","msm/cdsp/audio_pd" };
#endif /* AR_OSAL_USE_PD_NOTIFIER */

#define ADSP_LOADER_PATH    "/sys/kernel/boot_adsp/ssr"
#define PROC_PANIC_PATH     "/proc/sysrq-trigger"

#define AR_OSAL_SERVREG_CLIENT_NAME "apps/ar_osal"

int32_t g_init_done = 0;
struct ar_osal_service_node
{

    ar_osal_servreg_t          srv_handle;
    ar_osal_servreg_entry_type service;
    ar_osal_servreg_entry_type domain;
    void                         *cb_context;
    ar_osal_servreg_callback   cb_func;
    ar_osal_service_state_type srv_state;
#ifdef AR_OSAL_USE_PD_NOTIFIER
    PD_Notifier_Handle           *pd_handle;
#endif /* AR_OSAL_USE_PD_NOTIFIER */
#ifdef AR_OSAL_USE_MODEM_SSR
    pthread_t monitor_thread;
    int intPipe[2];
    int fd;
#endif
};

typedef struct ar_osal_service_node ar_osal_service_node;
#ifdef AR_OSAL_USE_MODEM_SSR
ar_osal_service_node modem_node;
ar_osal_mutex_t modem_node_mutex;
#define READY_TO_READ(p) ((p)->revents & (POLLIN|POLLPRI))
#define ERROR_IN_FD(p) ((p)->revents & (POLLERR|POLLHUP|POLLNVAL))
#endif
ar_osal_servreg_t serv_reg_handle;

#ifdef AR_OSAL_USE_PD_NOTIFIER
static ar_osal_service_state_type pd_event_to_ar_osal_pd_state(enum pd_event event)
{
    ar_osal_service_state_type ret = AR_OSAL_SERVICE_STATE_DOWN;
    switch (event) {
    case EVENT_PD_DOWN:
        ret = AR_OSAL_SERVICE_STATE_DOWN;
        break;
    case EVENT_PD_UP:
        ret = AR_OSAL_SERVICE_STATE_UP;
        break;
    default:
        ret = AR_OSAL_SERVICE_STATE_DOWN;
        break;
    }
    return ret;
}

static ar_osal_service_state_type pd_state_to_ar_osal_pd_state(pd_state state)
{
    ar_osal_service_state_type ret = AR_OSAL_SERVICE_STATE_DOWN;

    switch (state) {
    case SERVREG_NOTIF_SERVICE_STATE_DOWN_V01:
        ret = AR_OSAL_SERVICE_STATE_DOWN;
        break;
    case SERVREG_NOTIF_SERVICE_STATE_UP_V01:
        ret = AR_OSAL_SERVICE_STATE_UP;
        break;
    default:
        ret = AR_OSAL_SERVICE_STATE_DOWN;
    }
    return ret;
}

static void ar_osal_pd_notifier_cb(void *data, enum pd_event event)
{
    ar_osal_service_node *entry = (ar_osal_service_node *)data;
    ar_osal_service_state_type state = pd_event_to_ar_osal_pd_state(event);
    ar_osal_servreg_state_notify_payload_type notify_state;

    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "ar_osal_pd_notifier_cb state(%d) service(%s)",
	                                      state, entry->service.name );
    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "ar_osal_pd_notifier_cb state(%d) domain(%s)",
	                                      state, entry->domain.name );

    notify_state.service_state = state;
    memcpy((void*)&notify_state.service, (const void*)&entry->service, sizeof(ar_osal_servreg_entry_type));
    memcpy((void*)&notify_state.domain, (const void*)&entry->domain, sizeof(ar_osal_servreg_entry_type));


    if (entry)
    {
        if (entry->cb_func)
        {
            entry->cb_func((ar_osal_servreg_t)entry,
                AR_OSAL_SERVICE_STATE_NOTIFY,
                entry->cb_context,
                (void*)&notify_state,
                sizeof(notify_state));
        }
        entry->srv_state = state;
    }
}
#endif /* AR_OSAL_USE_PD_NOTIFIER */

#ifdef AR_OSAL_USE_MODEM_SSR

static void ar_osal_modem_notifier_cb(ar_osal_service_state_type state)
{
    ar_osal_servreg_state_notify_payload_type notify_state;
    ar_osal_servreg_callback cb_func_local = NULL;
    void *cb_context_local = NULL;

    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "ar_osal_modem_notifier_cb called");

    notify_state.service_state = state;
    strlcpy(notify_state.domain.name, domain_name[AR_MODEM_DSP], sizeof(notify_state.domain.name));
    notify_state.domain.instance_id = 74;

    ar_osal_mutex_lock(modem_node_mutex);
    cb_func_local = modem_node.cb_func;
    cb_context_local = modem_node.cb_context;
    ar_osal_mutex_unlock(modem_node_mutex);

    if (cb_func_local)
    {
        cb_func_local((ar_osal_servreg_t)&modem_node,
            AR_OSAL_SERVICE_STATE_NOTIFY,
            cb_context_local,
            (void*)&notify_state,
            sizeof(notify_state));
    }
}
char* read_state(int fd)
{
    struct stat buf;
    char *state = NULL;

    if (fstat(fd, &buf) < 0)
        return NULL;

    off_t pos = lseek(fd, 0, SEEK_CUR);
    off_t avail = buf.st_size - pos;
    if (avail <= 0) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "avail %ld", avail);
        return NULL;
    }

    state = (char *)calloc(avail+1, sizeof(char));
    if (!state)
        return NULL;

    ssize_t bytes = read(fd, state, avail);
    if (bytes <= 0) {
        free(state);
        return NULL;
    }

    // trim trailing whitespace
    while (bytes && isspace(*(state+bytes-1))) {
        *(state + bytes - 1) = '\0';
        --bytes;
    }
    lseek(fd, 0, SEEK_SET);
    return state;
}
int parse_snd_cards()
{
    int ret = AR_EOK;
    char path[128] = {0};
    int fd = -1;
    char *state = NULL;
    bool online;

    snprintf(path, sizeof(path), "/sys/kernel/snd_card/card_state", 0);

    if ((fd = open(path, O_RDONLY)) < 0) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Open %s failed", path);
        return AR_EUNSUPPORTED;
    }

    state = read_state(fd);
    if (!state) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Failed to read the state");
        close(fd);
        return AR_EUNSUPPORTED;
    }
    online = state && !strcmp(state, "1");
    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "initial state %s %d", state, online);

    modem_node.fd = fd;
    modem_node.srv_state = online ? AR_OSAL_SERVICE_STATE_UP : AR_OSAL_SERVICE_STATE_DOWN;

    free(state);
    return ret;
}
void on_sndcard_state_update()
{
    char *rd_buf;

    rd_buf = read_state(modem_node.fd);
    if (!rd_buf) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Error reading rd_buf");
        return;
    }

    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "new state %s", rd_buf);

    ar_osal_mutex_lock(modem_node_mutex);

    if (strstr(rd_buf, "0"))
        modem_node.srv_state = AR_OSAL_SERVICE_STATE_DOWN;
    else if (strstr(rd_buf, "1"))
        modem_node.srv_state = AR_OSAL_SERVICE_STATE_UP;
    else {
        ar_osal_mutex_unlock(modem_node_mutex);
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "unknown state");
        free(rd_buf);
        return;
    }

    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "state %d", modem_node.srv_state);

    ar_osal_mutex_unlock(modem_node_mutex);

    free(rd_buf);
    return;
}

void * monitor_node_state(void *arg)
{
    int i = 1;
    unsigned int num_poll_fds = 2/*pipe*/;
    struct pollfd *pfd = (struct pollfd *)calloc(num_poll_fds, sizeof(struct pollfd));
    if (!pfd) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Calloc failed for poll fds");
        return NULL;
    }

    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "Start monitor threadLoop.");

    pfd[0].fd = modem_node.intPipe[0];
    pfd[0].events = POLLPRI|POLLIN;
    pfd[i].fd = modem_node.fd;
    pfd[i].events = POLLPRI;

    while (1) {
        if (poll(pfd, num_poll_fds, -1) < 0) {
            int errno_ = errno;
            AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "poll() failed with err %s", strerror(errno_));
            switch (errno_) {
                case EINTR:
                case ENOMEM:
                    sleep(2);
                    continue;
                default:
                    /* above errors can be caused due to current system
                     * state .. any other error is not expected
                     */
                    AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "unxpected poll() system call failure");
                    break;
            }
        }
        AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "out of poll");

        // check if requested to exit
        if (READY_TO_READ(&pfd[0])) {
            char buf[2]={0};
            read(pfd[0].fd, buf, 1);
            if (!strcmp(buf, "Q"))
                break;
        } else if (ERROR_IN_FD(&pfd[0])) {
            /* do not consider for poll again
             * POLLERR - can this happen?
             * POLLHUP - adev must not close pipe
             * POLLNVAL - fd is valid
             */
            AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "unxpected error in pipe poll fd 0x%x",
                             pfd[0].revents);
            pfd[0].fd *= -1;
        }

        if (READY_TO_READ(&pfd[1])) {
            ar_osal_service_state_type current_state;
            on_sndcard_state_update();
            ar_osal_mutex_lock(modem_node_mutex);
            current_state = modem_node.srv_state;
            ar_osal_mutex_unlock(modem_node_mutex);
            ar_osal_modem_notifier_cb(current_state);
        } else if (ERROR_IN_FD(&pfd[1])) {
            /* do not consider for poll again
             * POLLERR - can this happen as we are reading from a fs?
             * POLLHUP - not valid for cardN/state
             * POLLNVAL - fd is valid
             */
            AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "unxpected error in card poll fd 0x%x",
                             pfd[1].revents);
            pfd[1].fd *= -1;
        }
    }
    free(pfd);
    return NULL;
}

#endif

/**
* \brief ar_osal_servreg_init
*        Initialize servreg interface.
*........Note:This API has to be called before any other API in this interface.
*........Should be called at least once and is expected to be serialized if called
*        multiple times.
* \return
*  0 -- Success
*  Nonzero -- Failure
*/
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_init(void)
{
    int32_t status = AR_EOK;
    // ar_osal_servreg_type_t* handle = NULL;
    if (g_init_done)
    {
        g_init_done++;
        return status;
    }
    g_init_done = 1;
#ifdef AR_OSAL_USE_PD_NOTIFIER
    pd_mapper_handle = servreg_alloc_DLHandle();
    if (!pd_mapper_handle)
    {
        status = AR_ENOMEMORY;
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "servreg allocation error status(%d)", status);
        goto end;
    }
#endif /* AR_OSAL_USE_PD_NOTIFIER */

#ifdef AR_OSAL_USE_MODEM_SSR
    if (pipe(modem_node.intPipe) < 0) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "failed to get pipe");
        status = AR_EFAILED;
        goto end;
    }

    status = ar_osal_mutex_create(&modem_node_mutex);
    if (status != AR_EOK) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "failed to create mutex");
        close(modem_node.intPipe[0]);
        close(modem_node.intPipe[1]);
        goto end;
    }

    if (parse_snd_cards()) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Unable to parse sound cards");
        status = AR_EFAILED;
        goto parse_sndcards_error;
    }

    if (pthread_create(&modem_node.monitor_thread, NULL, monitor_node_state, NULL) != 0) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Failed to create monitor thread");
        status = AR_EFAILED;
        close(modem_node.fd);
        goto parse_sndcards_error;
    }

    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "snd card monitor init done.");
    goto end;

parse_sndcards_error:
    ar_osal_mutex_destroy(modem_node_mutex);
    close(modem_node.intPipe[0]);
    close(modem_node.intPipe[1]);
#endif

end:
    // strlcpy(handle->client_name, AR_OSAL_SERVREG_TAG, sizeof(handle->client_name));
    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "ar_osal_servreg_init success status(%d)", status);
    return status;
}

/**
* \brief ar_osal_servreg_deinit
*        Uninitialize servreg interface.
*........Should be called in pair with ar_osal_servreg_init() and
*........should be a serialized call.
* \return
*  0 -- Success
*  Nonzero -- Failure
*/

_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_deinit(void)
{
    //uint32_t status = AR_EOK;
    // ar_osal_servreg_type_t* handle = (ar_osal_servreg_type_t*)pd_handle;
    g_init_done--;
    if (0 != g_init_done)
    {
        goto end;
    }
#ifdef AR_OSAL_USE_PD_NOTIFIER
    if (pd_mapper_handle)
    {
        servreg_free_DLHandle(pd_mapper_handle);
        pd_mapper_handle = NULL;
    }
#endif /* AR_OSAL_USE_PD_NOTIFIER */

#ifdef AR_OSAL_USE_MODEM_SSR
    write(modem_node.intPipe[1], "Q", 1);
    pthread_join(modem_node.monitor_thread, NULL);
    ar_osal_mutex_destroy(modem_node_mutex);
    close(modem_node.intPipe[0]);
    close(modem_node.intPipe[1]);
#endif
end:
    return AR_EOK;
}

/**
* \brief ar_osal_servreg_get_domainlist
*        Client to call this API to get a list of domains(msm/domain/subdomain) on which
*        a given service(provider/service) is supported.
*
* \param[in]  service: service(provider/service) for which domain(s) list is required.
* \param[in out] domain_list: service supported in domain(s), client to provide
*.                            payload buffer pointer.
* \param[in out] num_domains: Client to provide the num_domains to get the domain list.
*.                            Input NULL domain_list to get the number of domains
*                             for the given service if available in num_domains.
*
* \return
*  0 -- Success
*  Nonzero -- Failure
*  AR_ENOMEMORY- Failed due to insufficient memory, client to call the API again
*                  with required size as returned in num_domains.
*.
*/

_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_get_domainlist(_In_ ar_osal_servreg_entry_type *service,
    _Inout_opt_ ar_osal_servreg_entry_type *domain_list,
    _Inout_ uint32_t *num_domains)
{
    int32_t status = AR_EOK;
    if (NULL == service || NULL == num_domains)
    {
        status = AR_EBADPARAM;
        goto end;
    }
#ifdef AR_OSAL_USE_PD_NOTIFIER
    if (NULL == domain_list)
    {
        enum SR_Result_Enum rc = SR_RESULT_SUCCESS;
        rc = servreg_get_domainlist((char *)service->name, pd_mapper_handle);
        if (rc != SR_RESULT_SUCCESS)
        {
            status = AR_EFAILED;
            AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "servreg_get_domainlist error status(%d)"
                "servreg status(%d)", status, rc);
            goto end;
        }

        //return single domain
        *num_domains = servreg_get_numentries(pd_mapper_handle);
        status = AR_ENOMEMORY;
    }
    else if ((domain_list != NULL) && (num_domains > 0))
    {
        //copy domain details
        enum SR_Result_Enum rc = SR_RESULT_SUCCESS;
        uint32_t i = 0;
        //memcpy_s(domain_list->name, sizeof(domain_list->name), domain_name, sizeof(domain_name));
        for (i = 0; i < *num_domains; i++)
        {
            char* name;
            int instance = 0;
            int service_data_valid = 0;
            int service_data = 0;
            rc = servreg_get_entry(pd_mapper_handle,
                &name,
                &instance,
                &service_data_valid,
                &service_data,
                i);
            if (rc != SR_RESULT_SUCCESS)
            {
                status = AR_EFAILED;
                AR_LOG_ERR(AR_OSAL_SERVREG_TAG,
                    "servreg_get_domainlist entry(%d) error status(%d)"
                    "servreg status(%d)",
                    i, status, rc);
                goto end;
            }

            strlcpy((domain_list + i)->name, name, sizeof(domain_list->name));
            (domain_list + i)->instance_id = instance;

            AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "servreg_get_domainlist entry(%d)"
                " instance(%d) domain name(%s)",
                i, instance, (domain_list + i)->name);
        }
    }
    else
    {
        // service not found.
        status = AR_EBADPARAM;
    }
#else
    if (NULL == domain_list)
    {
        //return supported domains
        *num_domains = AR_SUB_SYS_ID_LAST;
        status = AR_ENOMEMORY;
        goto end;
    }

    //copy domain details
    strlcpy(domain_list[0].name, domain_name[AR_MODEM_DSP], sizeof(domain_list[0].name));
    strlcpy(domain_list[1].name, domain_name[AR_AUDIO_DSP], sizeof(domain_list[1].name));
    strlcpy(domain_list[2].name, domain_name[AR_APSS], sizeof(domain_list[2].name));
    strlcpy(domain_list[3].name, domain_name[AR_SENSOR_DSP], sizeof(domain_list[3].name));
    strlcpy(domain_list[4].name, domain_name[AR_COMPUTE_DSP], sizeof(domain_list[4].name));
    domain_list[0].instance_id = 74;
    domain_list[1].instance_id = 74;
    domain_list[2].instance_id = 74;
    domain_list[3].instance_id = 74;
    domain_list[4].instance_id = 74;
    *num_domains = AR_SUB_SYS_ID_LAST;
#endif

end:
    AR_LOG_INFO(AR_OSAL_SERVREG_TAG,
        "ar_osal_servreg_get_domainlist service(%s) status(0x%x)",
     service->name, status);
    return status;
}

/**
* \brief ar_osal_servreg_register
*        Service client(s) to register for the domain service state change notifications.
*
* \param[in opt]  cb_func: callback function pointer to get notifications on.
*.                         This is parameter is optional to Service provider registration.
* \param[in opt]  cb_context: callback function payload/context provided by client.
*.                         This is parameter is optional to Service provider registration.
* \param[in]  domain: domain of the service(msm/domain/subdomain) for which the
*.            state change notifications to be provided.
* \param[in]  service: service(provider/service) for which the
*.            state change notifications to be provided.
* \return
*  servreg_handle on success.
*  null on failure.
*/
_IRQL_requires_max_(PASSIVE_LEVEL)
ar_osal_servreg_t ar_osal_servreg_register(_In_ ar_osal_client_type  client_type __unused,
    _In_opt_ ar_osal_servreg_callback cb_func,
    _In_opt_ void *cb_context,
    _In_ ar_osal_servreg_entry_type *domain,
    _In_ ar_osal_servreg_entry_type *service)
{
#if  defined(AR_OSAL_USE_MODEM_SSR)

    if (NULL == domain || NULL == service) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Invalid Input params domain/service");
        return NULL;
    }

    if (!strcmp(domain->name, "msm/mdsp/audio_pd")) {
        AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "Register modem SSR functions,\
                        Domain name %s", domain->name);

        ar_osal_mutex_lock(modem_node_mutex);
        modem_node.cb_func = cb_func;
        modem_node.cb_context = cb_context;
        ar_osal_mutex_unlock(modem_node_mutex);
    }
    return (&modem_node);

#elif defined(AR_OSAL_USE_PD_NOTIFIER)
    int32_t status = AR_EOK;
    ar_osal_service_node* srv_reg_handle = NULL;
    enum pd_rcode pd_rc = PD_NOTIFIER_FAIL;
    pd_state state = SERVREG_NOTIF_SERVICE_STATE_DOWN_V01;

    if (NULL == domain || NULL == service || NULL == cb_func)
    {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Invalid input params");
        goto end;
    }
    srv_reg_handle = (ar_osal_service_node*)malloc(sizeof(ar_osal_service_node));
    if (!srv_reg_handle)
    {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "handle allocation failed");
        goto end;
    }

    memset((void*)srv_reg_handle, 0, sizeof(ar_osal_service_node));
    memcpy(&srv_reg_handle->service, service, sizeof(ar_osal_servreg_entry_type));
    memcpy(&srv_reg_handle->domain, domain, sizeof(ar_osal_servreg_entry_type));

    srv_reg_handle->cb_func = cb_func;
    srv_reg_handle->cb_context = cb_context;
    snprintf(pd_service_name, sizeof(pd_service_name), "%s/%s", domain->name, service->name);

    srv_reg_handle->pd_handle = pd_notifier_alloc(pd_service_name,
        AR_OSAL_SERVREG_CLIENT_NAME,
        domain->instance_id,
        ar_osal_pd_notifier_cb,
        (void*)srv_reg_handle);

    if (!srv_reg_handle->pd_handle)
    {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "pd_notifier_alloc failed");
        free(srv_reg_handle);
        srv_reg_handle = NULL;
        goto end;
    }

    pd_rc = pd_notifier_register(srv_reg_handle->pd_handle, &state);

    if (pd_rc != PD_NOTIFIER_SUCCESS) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "pd_notifier_register failed for %s, status(%d)",
            pd_service_name, pd_rc);
        pd_notifier_free(srv_reg_handle->pd_handle);
        free(srv_reg_handle);
        srv_reg_handle = NULL;
        goto end;
    }
    else {
        AR_LOG_INFO(AR_OSAL_SERVREG_TAG,
            "Successfully registered %s.  Curr state is %s state (0x%08x)",
            pd_service_name,
            (state == EVENT_PD_UNKNOWN) ? "unknown" :
            ((state == EVENT_PD_UP) ? "up" :
            ((state == EVENT_PD_DOWN) ? "down" : "out of range")),
            state);
        srv_reg_handle->srv_state = pd_state_to_ar_osal_pd_state(state);
    }

end:
    return (ar_osal_servreg_t)srv_reg_handle;
#else
    return 1;
#endif
}

/**
* \brief ar_osal_servreg_deregister
*        Service client(s) to deregister for the service state change notifications.
*
* \param[in]  servreg_handle: interface handle returned by ar_osal_servreg_allocate_handle().
*
* \return
*  0 -- Success
*  Nonzero -- Failure
*/
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_deregister(_In_ ar_osal_servreg_t servreg_handle)
{
#if defined(AR_OSAL_USE_PD_NOTIFIER)
    int32_t status = AR_EOK;
    enum pd_rcode pd_rc = PD_NOTIFIER_FAIL;
    ar_osal_service_node* srv_reg_handle = (ar_osal_service_node*)servreg_handle;
    if (NULL == servreg_handle)
    {
        status = AR_EBADPARAM;
        goto end;
    }

    pd_rc = pd_notifier_deregister(srv_reg_handle->pd_handle);
    if (pd_rc != PD_NOTIFIER_SUCCESS) {
        status = AR_EFAILED;
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "pd_notifier_deregister error: status(%d)"
            "pd_notifier status(%d)",
            status, pd_rc);
        pd_notifier_free(srv_reg_handle->pd_handle);
        free(srv_reg_handle);
        srv_reg_handle = NULL;
        goto end;
    }
    pd_notifier_free(srv_reg_handle->pd_handle);
    free(srv_reg_handle);
    srv_reg_handle = NULL;
end:
    return status;
#elif defined(AR_OSAL_USE_MODEM_SSR)
    ar_osal_mutex_lock(modem_node_mutex);
    modem_node.cb_func = NULL;
    modem_node.cb_context = NULL;
    ar_osal_mutex_unlock(modem_node_mutex);
    return AR_EOK;
#else
    return 1;
#endif
}

/**
* \brief ar_osal_servreg_set_state
*        Service provider to call this API to register its service states(UP/DOWN).
*        This API to be used only by the service provider(msm/domain/subdomain/provider/service)
*        and not by service client(s).
*
* \param[in]  servreg_handle: interface handle returned by ar_osal_servreg_allocate_handle().
* \param[in]  state: new service state for service registered using ar_osal_servreg_register().
*.
* \return
*  0 -- Success
*  Nonzero -- Failure
*/
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_set_state(_In_ ar_osal_servreg_t servreg_handle __unused,
    _In_ ar_osal_service_state_type state __unused)
{
    return AR_ENOTIMPL;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

_IRQL_requires_max_(PASSIVE_LEVEL)
static int32_t ar_osal_servreg_ssr() {
#ifdef PROPERTY_TRIGGER
    if (property_set("vendor.audio.ssr.trigger", "1")) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "%s: set property failed", __func__);
        return -1;
    }
    return 0;
#else
    int fd_dsplder;
    int32_t rc = 0;

    fd_dsplder = open(ADSP_LOADER_PATH, O_WRONLY);

    if(fd_dsplder < 0) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "%s: open (%s) fail - %s (%d)",
		   __func__, ADSP_LOADER_PATH, strerror(errno), errno);
	rc = errno;
    } else if (write(fd_dsplder, "1", 1) < 0) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "%s: write (%s) fail - %s (%d)",
		   __func__, ADSP_LOADER_PATH, strerror(errno), errno);
	rc = errno;
    }

    return rc;
#endif
}

/**
* \brief induce panic to crash the system
*
*/
void ar_osal_panic()
{
#ifdef PROPERTY_TRIGGER
    if (property_set("vendor.audio.crash.trigger", "1"))
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "%s: set property failed", __func__);
#else
    char panic_set ='c';
    int fd_sysrq = 0;

    fd_sysrq = open(PROC_PANIC_PATH, O_WRONLY);

    if(fd_sysrq < 0) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "%s: open (%s) fail - %s (%d)", __func__,
                   PROC_PANIC_PATH, strerror(errno), errno);
	//ignore if panic path can't be opened
    } else if (write(fd_sysrq, &panic_set, 1) < 0) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG,"%s: write (%s) fail - %s (%d)", __func__,
		   PROC_PANIC_PATH, strerror(errno), errno);
    }
#endif
}

/**
* \brief ar_osal_servreg_restart_service
*        HLOS calls this API to trigger a restart (PDR or SSR) on a given
*        processor
*
* \param[in]  servreg_handle: interface handle returned by
*             ar_osal_servreg_register() which identifies the desired processor
*
* \return
*  0 -- Success
*  Nonzero -- Failure
*/
int32_t ar_osal_servreg_restart_service(ar_osal_servreg_t servreg_handle)
{
#ifndef AR_OSAL_USE_PD_NOTIFIER
    return 1;
#else
    int32_t rc = AR_EOK;
    int32_t ssr_rc;
    enum pd_rcode pd_rc;
    ar_osal_service_node* serv_reg_handle = (ar_osal_service_node*)servreg_handle;
#ifdef AR_OSAL_USE_CUTILS
    char value[256] = {0};

    property_get("persist.vendor.audio.induce_crash", value, "");
    if (!strncmp("true", value, sizeof("true")))
        ar_osal_panic();

    property_get("persist.vendor.audio.spf_restart", value, "");
    if (strncmp("true", value, sizeof("true"))) {
        AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "SPF restart feature disabled\n");
        return rc;
    }
#endif

    AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "Restarting service %d", servreg_handle);

    if (serv_reg_handle == NULL)
    {
        rc = AR_EBADPARAM;
        goto end;
    }

    pd_rc = pd_notifier_restart_pd(serv_reg_handle->pd_handle);
    if (pd_rc != PD_NOTIFIER_SUCCESS) {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG,
		   "Failed to restart audio service rc(%d) perform SSR\n", pd_rc);
	ssr_rc = ar_osal_servreg_ssr();
        if (ssr_rc) {
	    AR_LOG_ERR(AR_OSAL_SERVREG_TAG,
                       "Failed to restart audio DSP rc(%d)\n", ssr_rc);
	    rc = AR_EUNEXPECTED;
	}
    }

end:
    return rc;
#endif
}
