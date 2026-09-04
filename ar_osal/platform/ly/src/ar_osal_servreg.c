/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include "ar_osal_types.h"
#include "ar_osal_servreg.h"
#include "ar_util_list.h"
#include "ar_osal_mutex.h"
#include "ar_osal_error.h"
#include "ar_osal_log.h"
#include "ar_osal_mem_op.h"
#include "ar_osal_sys_id.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ssr_api.h"
#include "ssr_events.h"

#define AR_OSAL_SERVREG_TAG                 "AOSR"
#define AR_OSAL_SERVREG_AUDIO_SERVICE       "avs/audio"
#define AR_OSAL_SERVREG_AUDIO_INSTANCE_ID   74U


/* Per-DSP configuration entry: subsystem ID, domain name, and SSR bitmask */
typedef struct {
    uint32_t sys_id;
    char_t   domain_name[24];
    uint32_t ss_id_bit;
    char_t   dsp_name[8];
} audio_dsp_cfg_t;

static const audio_dsp_cfg_t g_dsp_cfg[] = {
    { AR_AUDIO_DSP,  "msm/adsp/audio_pd",  SS_ID_LPASS, "ADSP0" },
    { AR_AUDIO_DSP1, "msm/adsp1/audio_pd", SS_ID_ADSP1, "ADSP1" },
    { AR_AUDIO_DSP2, "msm/adsp2/audio_pd", SS_ID_ADSP2, "ADSP2" },
};

#define DSP_CFG_COUNT (sizeof(g_dsp_cfg) / sizeof(g_dsp_cfg[0]))

static void   *g_ssr_cb_handle       = NULL;

typedef struct {
    ar_list_t        list_handle;
    uint32_t         num_nodes;
    ar_osal_mutex_t  lock;
} servreg_handle_pool_type;

typedef struct {
    ar_list_node_t             srv_node;
    ar_osal_servreg_t          srv_handle;
    ar_osal_servreg_entry_type  service;
    ar_osal_servreg_entry_type  domain;
    void                      *cb_context;
    ar_osal_servreg_callback    cb_func;
    ar_osal_client_type         client_type;
    ar_osal_service_state_type  srv_state;
} service_node;

#define get_struc_base(p, type, member) ((type *)((int8_t *)(p) - (uintptr_t)(&((type *)0)->member)))

static servreg_handle_pool_type servreg_handle_pool = { 0 };
static int32_t g_init_done = 0;

PAGED_FUNCTIONS_START

/**
 * \brief notify_clients_for_domain
 *        Notify only the registered client whose domain matches domain_name.
 *
 * \param[in] domain_name: Domain string to match (e.g. "msm/adsp1/audio_pd").
 * \param[in] state:       The new service state to deliver.
 */
static void notify_clients_for_domain(const char *domain_name,
                                      ar_osal_service_state_type state)
{
    ar_list_node_t *list_node = NULL;
    service_node *node = NULL;
    ar_osal_servreg_state_notify_payload_type payload;

    ar_list_for_each_entry(list_node, &servreg_handle_pool.list_handle)
    {
        node = get_struc_base(list_node, service_node, srv_node);
        if (node != NULL && node->cb_func != NULL &&
            strcmp(node->domain.name, domain_name) == 0)
        {
            payload.service_state = state;
            ar_mem_cpy(&payload.service, sizeof(payload.service), &node->service, sizeof(node->service));
            ar_mem_cpy(&payload.domain, sizeof(payload.domain), &node->domain, sizeof(node->domain));

            AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "Notifying client: Service(%s) on Domain(%s) state(%d)",
                        node->service.name, node->domain.name, state);

            node->cb_func(node->srv_handle, AR_OSAL_SERVICE_STATE_NOTIFY, node->cb_context, &payload, sizeof(payload));
            node->srv_state = state;
        }
    }
}

/**
 * \brief arosal_ssr_event_cb
 *        Per-DSP SSR event handler. Fires the registered callback for each
 *        DSP domain that matches the incoming ss_id_mask. UP/DOWN gating for
 *        multi-DSP recovery is handled at the AGM level.
 *
 * \param[in] ss_id_mask: Mask of subsystems affected by the event.
 * \param[in] event_id:   The SSR event (Start/Complete).
 * \param[in] ctx:        User context (unused).
 *
 * \return AR_EOK on success.
 */
int32_t arosal_ssr_event_cb(uint32_t ss_id_mask, uint32_t event_id, void *ctx)
{
    (void)ctx;
    ar_osal_service_state_type state;
    uint32_t i;

    if (event_id == SSR_EVENT_RESTART_START)
        state = AR_OSAL_SERVICE_STATE_DOWN;
    else if (event_id == SSR_EVENT_RESTART_COMPLETE)
        state = AR_OSAL_SERVICE_STATE_UP;
    else
        return AR_EOK;

    ar_osal_mutex_lock(servreg_handle_pool.lock);

    for (i = 0; i < DSP_CFG_COUNT; i++)
    {
        if (ss_id_mask & g_dsp_cfg[i].ss_id_bit)
        {
            AR_LOG_INFO(AR_OSAL_SERVREG_TAG, "SSR %s received for %s, notifying domain %s",
                        (event_id == SSR_EVENT_RESTART_START) ? "Start" : "Complete",
                        g_dsp_cfg[i].dsp_name, g_dsp_cfg[i].domain_name);
            notify_clients_for_domain(g_dsp_cfg[i].domain_name, state);
        }
    }

    ar_osal_mutex_unlock(servreg_handle_pool.lock);
    return AR_EOK;
}

/**
 * \brief ar_servreg_validate_service_name
 *        Internal validation for supported service names.
 *
 * \param[in] serv_name: Pointer to the service name string.
 *
 * \return AR_EOK if valid, AR_ENOTEXIST otherwise.
 */
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_servreg_validate_service_name(_In_ uint8_t *serv_name)
{
    PAGED_FUNCTION();

    if (serv_name == NULL)
    {
        return AR_EBADPARAM;
    }

    return (strcmp((char *)serv_name, AR_OSAL_SERVREG_AUDIO_SERVICE) == 0) ? AR_EOK : AR_ENOTEXIST;
}

/**
 * \brief ar_osal_servreg_init
 *        Initialize the service registry module and global SSR callbacks.
 *
 * \return AR_EOK on success, error code otherwise.
 */
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_init(void)
{
    PAGED_FUNCTION();
    int32_t status = AR_EOK;

    if (g_init_done != 0)
    {
        g_init_done++;
        return AR_EOK;
    }

    memset(&servreg_handle_pool, 0, sizeof(servreg_handle_pool));

    status = ar_list_init(&servreg_handle_pool.list_handle, NULL, NULL);
    if (status != AR_EOK)
    {
        goto end;
    }

    status = ar_osal_mutex_create(&servreg_handle_pool.lock);
    if (status != AR_EOK)
    {
        goto end;
    }

    /* Build SSR mask from the product DSP table */
    uint64_t ss_mask = 0;
    for (uint32_t i = 0; i < DSP_CFG_COUNT; i++)
        ss_mask |= g_dsp_cfg[i].ss_id_bit;
    uint64_t mask = (ss_mask << SS_ID_SHIFT) |
                    (SSR_EVENT_RESTART_START | SSR_EVENT_RESTART_COMPLETE);

    status = ssr_register_callback_events(0, arosal_ssr_event_cb, mask, &g_ssr_cb_handle, "audio_pd", NULL);
    if (status != AR_EOK)
    {
        AR_LOG_ERR(AR_OSAL_SERVREG_TAG, "Global SSR registration failed: %d", status);
        goto end;
    }

    g_init_done = 1;

end:
    if (status != AR_EOK)
    {
        g_init_done = 0;
    }
    return status;
}

/**
 * \brief ar_osal_servreg_deinit
 *        Cleanup module resources and unregister global SSR callbacks.
 *
 * \return AR_EOK on success.
 */
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_deinit(void)
{
    PAGED_FUNCTION();

    if (--g_init_done != 0)
    {
        return AR_EOK;
    }

    ar_osal_mutex_lock(servreg_handle_pool.lock);

    if (g_ssr_cb_handle != NULL)
    {
        ssr_unregister_callback(g_ssr_cb_handle);
        g_ssr_cb_handle = NULL;
    }

    while (servreg_handle_pool.num_nodes > 0)
    {
        ar_list_node_t *ln = NULL;
        ar_list_for_each_entry(ln, &servreg_handle_pool.list_handle)
        {
            service_node *node = get_struc_base(ln, service_node, srv_node);
            ar_list_delete(&servreg_handle_pool.list_handle, ln);
            servreg_handle_pool.num_nodes--;
            free(node);
            break;
        }
    }

    ar_osal_mutex_unlock(servreg_handle_pool.lock);
    ar_osal_mutex_destroy(servreg_handle_pool.lock);
    memset(&servreg_handle_pool, 0, sizeof(servreg_handle_pool));

    return AR_EOK;
}

/**
 * \brief ar_osal_servreg_get_domainlist
 *        Retrieves the list of supported audio domains.
 *
 * \param[in]     service:     Service to query.
 * \param[in,out] domain_list: Pointer to domain list buffer.
 * \param[in,out] num_domains: Input buffer size / Output domain count.
 *
 * \return AR_EOK on success, AR_ENOMEMORY if buffer is too small.
 */
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_get_domainlist(_In_ ar_osal_servreg_entry_type *service, _Inout_opt_ ar_osal_servreg_entry_type *domain_list, _Inout_ uint32_t *num_domains)
{
    PAGED_FUNCTION();
    if (service == NULL || num_domains == NULL)
    {
        return AR_EBADPARAM;
    }

    if (ar_servreg_validate_service_name((uint8_t *)service->name) != AR_EOK)
    {
        return AR_ENOTEXIST;
    }

    if (domain_list == NULL || *num_domains < DSP_CFG_COUNT)
    {
        *num_domains = DSP_CFG_COUNT;
        return AR_ENOMEMORY;
    }

    for (uint32_t i = 0; i < DSP_CFG_COUNT; i++)
    {
        ar_mem_cpy(domain_list[i].name, sizeof(domain_list[i].name),
                   g_dsp_cfg[i].domain_name, strlen(g_dsp_cfg[i].domain_name) + 1);
        domain_list[i].instance_id = AR_OSAL_SERVREG_AUDIO_INSTANCE_ID;
    }

    *num_domains = DSP_CFG_COUNT;
    return AR_EOK;
}

/**
 * \brief ar_osal_servreg_register
 *        Register a client/provider for service state notifications.
 *
 * \param[in] type:    Listener or Provider.
 * \param[in] cb:      Callback function.
 * \param[in] ctx:     User callback context.
 * \param[in] domain:  Domain to monitor.
 * \param[in] service: Service to monitor.
 *
 * \return Handle to the registration node on success.
 */
_IRQL_requires_max_(PASSIVE_LEVEL)
ar_osal_servreg_t ar_osal_servreg_register(ar_osal_client_type type, ar_osal_servreg_callback cb, void *ctx, ar_osal_servreg_entry_type *domain, ar_osal_servreg_entry_type *service)
{
    PAGED_FUNCTION();

    if (domain == NULL || service == NULL)
    {
        return NULL;
    }

    service_node *node = calloc(1, sizeof(service_node));
    if (node == NULL)
    {
        return NULL;
    }

    ar_list_init_node(&node->srv_node);
    node->cb_func = cb;
    node->cb_context = ctx;
    node->client_type = type;
    node->srv_state = AR_OSAL_SERVICE_STATE_DOWN;

    ar_mem_cpy(node->service.name, sizeof(node->service.name), service->name, sizeof(service->name));
    ar_mem_cpy(node->domain.name, sizeof(node->domain.name), domain->name, sizeof(domain->name));

    node->service.instance_id = service->instance_id;
    node->domain.instance_id = domain->instance_id;
    node->srv_handle = node;

    ar_osal_mutex_lock(servreg_handle_pool.lock);
    ar_list_add_tail(&servreg_handle_pool.list_handle, &node->srv_node);
    servreg_handle_pool.num_nodes++;
    ar_osal_mutex_unlock(servreg_handle_pool.lock);

    return node;
}

/**
 * \brief ar_osal_servreg_deregister
 *        Unregister a handle and stop notifications.
 *
 * \param[in] handle: The registration handle.
 *
 * \return AR_EOK on success.
 */
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_deregister(_In_ ar_osal_servreg_t handle)
{
    PAGED_FUNCTION();

    if (handle == NULL)
    {
        return AR_EBADPARAM;
    }

    int32_t status = AR_ENOTEXIST;

    ar_osal_mutex_lock(servreg_handle_pool.lock);

    ar_list_node_t *ln = NULL;
    ar_list_for_each_entry(ln, &servreg_handle_pool.list_handle)
    {
        service_node *node = get_struc_base(ln, service_node, srv_node);
        if (handle == node->srv_handle)
        {
            ar_list_delete(&servreg_handle_pool.list_handle, ln);
            servreg_handle_pool.num_nodes--;
            free(node);
            status = AR_EOK;
            break;
        }
    }
    ar_osal_mutex_unlock(servreg_handle_pool.lock);
    return status;
}

/**
 * \brief ar_osal_servreg_set_state
 *        Manually set service state and notify matching clients.
 *
 * \param[in] handle: Provider handle.
 * \param[in] state:  New state (UP/DOWN).
 *
 * \return AR_EOK on success.
 */
_IRQL_requires_max_(PASSIVE_LEVEL)
int32_t ar_osal_servreg_set_state(_In_ ar_osal_servreg_t handle, _In_ ar_osal_service_state_type state)
{
    PAGED_FUNCTION();
    int32_t status = AR_ENOTEXIST;
    service_node *provider = NULL, *client = NULL;
    ar_list_node_t *ln = NULL;
    ar_osal_servreg_state_notify_payload_type payload;

    if (handle == NULL) return AR_EBADPARAM;

    ar_osal_mutex_lock(servreg_handle_pool.lock);
    ar_list_for_each_entry(ln, &servreg_handle_pool.list_handle)
    {
        provider = get_struc_base(ln, service_node, srv_node);
        if (handle == provider->srv_handle) { provider->srv_state = state; status = AR_EOK; break; }
    }

    if (status == AR_EOK)
    {
        ar_list_for_each_entry(ln, &servreg_handle_pool.list_handle)
        {
            client = get_struc_base(ln, service_node, srv_node);
            if (client->client_type == AR_OSAL_CLIENT_LISTENER &&
                strcmp(client->service.name, provider->service.name) == 0 &&
                strcmp(client->domain.name, provider->domain.name) == 0)
            {
                client->srv_state = state;
                if (client->cb_func)
                {
                    payload.service_state = state;
                    ar_mem_cpy(&payload.service, sizeof(payload.service), &client->service, sizeof(client->service));
                    ar_mem_cpy(&payload.domain, sizeof(payload.domain), &client->domain, sizeof(client->domain));
                    client->cb_func(client->srv_handle, AR_OSAL_SERVICE_STATE_NOTIFY, client->cb_context, &payload, sizeof(payload));
                }
            }
        }
    }
    ar_osal_mutex_unlock(servreg_handle_pool.lock);
    return status;
}

/**
 * \brief ar_osal_servreg_restart_service
 *        Placeholder for service restart capability.
 *
 * \return AR_EOK.
 */
int32_t ar_osal_servreg_restart_service(ar_osal_servreg_t handle)
{
	return AR_EOK;
}

PAGED_FUNCTIONS_END
