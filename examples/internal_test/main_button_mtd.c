#include <assert.h>
#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/platform/logging.h>
#include <openthread/thread.h>
#include <openthread/udp.h>
#include <openthread/logging.h>

#include "openthread-system.h"
#include "cli/cli_config.h"
#include "common/code_utils.hpp"


#include "project_config.h"
#include "cm3_mcu.h"
#include "rf_mcu.h"
#include "bsp.h"
#include "bsp_console.h"
#include "bsp_uart.h"
#include "util_log.h"

#if OPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT
#define DEF_CHANNEL 6
#else
#define DEF_CHANNEL 21
#endif

otInstance *g_app_instance;
otUdpSocket socket;
otSockAddr  sockaddr;
otBufferInfo aBufferInfo;

extern void otAppCliInit(otInstance *aInstance);
static bool g_rx_on_when_Idle = true;

static void _NetIFStateChange(uint32_t aFlags, void *aContext)
{
    uint8_t show_ip = 0;
    otLinkModeConfig config;
    if ((aFlags & OT_CHANGED_THREAD_ROLE) != 0)
    {
        otDeviceRole changeRole = otThreadGetDeviceRole(g_app_instance);
        g_rx_on_when_Idle = true;
        switch (changeRole)
        {
        case OT_DEVICE_ROLE_LEADER:
            info("Change to leader \r\n");
            break;

        case OT_DEVICE_ROLE_ROUTER:
            info("Change to router \r\n");
            break;
        case OT_DEVICE_ROLE_CHILD:
            show_ip = 1;
            config = otThreadGetLinkMode(g_app_instance);
            if(config.mRxOnWhenIdle == false)
            {
                g_rx_on_when_Idle = false;
            }            
            info("Change to child \r\n");
            break;
        default:
            break;
        }

        if (show_ip)
        {
            const otNetifAddress *unicastAddress = otIp6GetUnicastAddresses(g_app_instance);

            for (const otNetifAddress *addr = unicastAddress; addr; addr = addr->mNext)
            {
                char string[OT_IP6_ADDRESS_STRING_SIZE];

                otIp6AddressToString(&addr->mAddress, string, sizeof(string));
                info("\t%s\n", string);
            }
        }
    }
}
static void _SetNetConfig(otInstance *aInstance)
{
    static char aNetworkName[] = "Thread_Cert";
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = { 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00};
#if 0
    uint8_t nwkkey[OT_NETWORK_KEY_SIZE] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
                                          };
#else
    uint8_t nwkkey[OT_NETWORK_KEY_SIZE] = { 0xfe, 0x83, 0x44, 0x8a, 0x67, 0x29, 0xfe, 0xab,
                                            0xab, 0xfe, 0x29, 0x67, 0x8a, 0x44, 0x83, 0xfe
                                          };
#endif
    uint8_t meshLocalPrefix[OT_MESH_LOCAL_PREFIX_SIZE] = { 0xfd, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00 };
    uint8_t aPSKc[OT_PSKC_MAX_SIZE] = { 0x74, 0x68, 0x72, 0x65,
                                        0x61, 0x64, 0x6a, 0x70,
                                        0x61, 0x6b, 0x65, 0x74,
                                        0x65, 0x73, 0x74, 0x00
                                      };


    otError error;
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    aDataset.mActiveTimestamp.mSeconds = 1;
    aDataset.mComponents.mIsActiveTimestampPresent = true;

    aDataset.mChannel = DEF_CHANNEL;

    aDataset.mComponents.mIsChannelPresent = true;

    aDataset.mPanId = (otPanId)0xbee0;
    aDataset.mComponents.mIsPanIdPresent = true;

    memcpy(aDataset.mExtendedPanId.m8, extPanId, OT_EXT_PAN_ID_SIZE);
    aDataset.mComponents.mIsExtendedPanIdPresent = true;

    memcpy(aDataset.mNetworkKey.m8, nwkkey, OT_NETWORK_KEY_SIZE);
    aDataset.mComponents.mIsNetworkKeyPresent = true;

    memcpy(aDataset.mNetworkName.m8, aNetworkName, 12);
    aDataset.mComponents.mIsNetworkNamePresent = true;

    memcpy(aDataset.mMeshLocalPrefix.m8, meshLocalPrefix, OT_MESH_LOCAL_PREFIX_SIZE);
    aDataset.mComponents.mIsMeshLocalPrefixPresent = true;


    error = otDatasetSetActive(aInstance, &aDataset);
    if (error != OT_ERROR_NONE)
    {
        info("otDatasetSetActive failed with %d %s\r\n", error, otThreadErrorToString(error));
    }
}

static void _sleep_init(otInstance *aInstance)
{
    otError error;

    otLinkModeConfig config;
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    error = otLinkCslSetChannel(aInstance, DEF_CHANNEL);
    if (error != OT_ERROR_NONE)
    {
        err("otLinkCslSetChannel failed with %d %s\r\n", error, otThreadErrorToString(error));
    }
    error = otLinkCslSetPeriod(aInstance, 31250);
    if (error != OT_ERROR_NONE)
    {
        err("otLinkCslSetPeriod failed with %d %s\r\n", error, otThreadErrorToString(error));
    }
    error = otLinkCslSetTimeout(aInstance, 100);
    if (error != OT_ERROR_NONE)
    {
        err("otLinkCslSetTimeout failed with %d %s\r\n", error, otThreadErrorToString(error));
    }
#else
    error = otLinkSetPollPeriod(aInstance, 5000);
    if (error != OT_ERROR_NONE)
    {
        err("otLinkSetPollPeriod failed with %d %s\r\n", error, otThreadErrorToString(error));
    }
#endif
    config.mRxOnWhenIdle = false;
    config.mNetworkData = 0;
    config.mDeviceType = 0;

    error = otThreadSetLinkMode(aInstance, config);

    if (error != OT_ERROR_NONE)
    {
        err("otThreadSetLinkMode failed with %d %s\r\n", error, otThreadErrorToString(error));
    }

}

void otTaskletsSignalPending(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    //Lpm_Low_Power_Mask(LOW_POWER_MASK_BIT_RESERVED31);
}

void udp_data_send()
{    
    Lpm_Low_Power_Mask(LOW_POWER_MASK_BIT_RESERVED29);
    otError     error;
    otMessage *message = NULL;
    otMessageInfo messageInfo;
    otMessageSettings messageSettings = {true, OT_MESSAGE_PRIORITY_NORMAL};
    char peer_addr[8] = "ff03::01";
    uint8_t buffer[76];

    memset(&buffer,0xff,sizeof(buffer));
    memset(&messageInfo, 0, sizeof(messageInfo));
    

    messageInfo.mPeerPort = sockaddr.mPort;    
    otIp6AddressFromString(peer_addr,&messageInfo.mPeerAddr);

    message = otUdpNewMessage(g_app_instance, &messageSettings);
    otMessageAppend(message, buffer, sizeof(buffer));
    error = otUdpSend(g_app_instance,&socket,message,&messageInfo);
    
    if(error == 0) message = NULL;

    if (message != NULL) otMessageFree(message);
    
}

void handleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aContext);
    OT_UNUSED_VARIABLE(aMessage);
    OT_UNUSED_VARIABLE(aMessageInfo);
}

void udp_init()
{
    otError     error;

    error = otUdpOpen(g_app_instance, &socket, handleUdpReceive, NULL);
    assert(error == OT_ERROR_NONE);

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.mPort = 5678;

    error = otUdpBind(g_app_instance, &socket, &sockaddr, OT_NETIF_THREAD);
    assert(error == OT_ERROR_NONE);
}

void buuton_event_callback(bsp_event_t even)
{
    if(BSP_EVENT_BUTTONS_0 == even)
    { 
        sys_set_retention_reg(1,1);  
    }
}

int main(int argc, char *argv[])
{
    otSysInit(argc, argv);
    
    g_app_instance = otInstanceInitSingle();

    assert(g_app_instance); 
    
    /* low power mode init */
    Lpm_Set_Low_Power_Level(LOW_POWER_LEVEL_SLEEP3);
    Lpm_Enable_Low_Power_Wakeup((LOW_POWER_WAKEUP_GPIO0));

    bsp_init(BSP_INIT_BUTTONS, buuton_event_callback);
    
    udp_init();

    otAppCliInit(g_app_instance);
#if (MODULE_ENABLE(SUPPORT_DEBUG_CONSOLE_CLI))
    cli_console_init();
#endif

#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
    otLoggingSetLevel(OT_LOG_LEVEL_WARN);
#endif
    
    _sleep_init(g_app_instance);
    _SetNetConfig(g_app_instance);
    otIp6SetEnabled(g_app_instance, true);
    otThreadSetEnabled(g_app_instance, true);
    info("Thread Init ability button MTD \n" );
    otSetStateChangedCallback(g_app_instance, _NetIFStateChange, NULL);
    uint32_t udp_send = 0;
    while (!otSysPseudoResetWasRequested())
    {
        otTaskletsProcess(g_app_instance);
        otSysProcessDrivers(g_app_instance);  
        if(g_rx_on_when_Idle == false)
        {   
            sys_get_retention_reg(1,&udp_send); 
            if(udp_send)
            {
                udp_data_send();
                sys_set_retention_reg(1,0);
            }
            else 
            {
                otMessageGetBufferInfo(g_app_instance,&aBufferInfo);
                if(aBufferInfo.m6loSendQueue.mNumMessages == 0)
                {
                    Lpm_Low_Power_Unmask(LOW_POWER_MASK_BIT_RESERVED29);
                }
            }

            Lpm_Low_Power_Unmask(LOW_POWER_MASK_BIT_RESERVED31);
            Lpm_Enter_Low_Power_Mode();
        }       
#if (MODULE_ENABLE(SUPPORT_DEBUG_CONSOLE_CLI))
        cli_console_proc();
#endif
    }

    otInstanceFinalize(g_app_instance);

    return 0;
}
