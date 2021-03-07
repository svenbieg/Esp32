//================
// WiFiHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <esp_event.h>
#include <esp_wifi.h>
#include "Network/WiFi/WiFiConnection.h"
#include "WiFiHelper.h"


//===========
// Namespace
//===========

namespace Network {
	namespace WiFi {


//=========
// Globals
//=========

esp_netif_t* pAccessPoint=nullptr;
esp_netif_t* pStation=nullptr;

BYTE RTC_DATA_ATTR WiFiChannel=0;


//===========
// Callbacks
//===========

VOID IpCallback(VOID* parg, esp_event_base_t pevent, INT id, VOID* pdata)
{
auto hcon=WiFiConnection::Current;
if(!hcon)
	return;
switch(id)
	{
	case IP_EVENT_STA_GOT_IP:
		{
		BYTE uch=0;
		wifi_second_chan_t usec=WIFI_SECOND_CHAN_NONE;
		if(esp_wifi_get_channel(&uch, &usec)==ESP_OK)
			{
			DebugPrint("WiFi Channel %u\r\n", uch);
			WiFiChannel=uch;
			}
		auto pparam=(ip_event_got_ip_t*)pdata;
		WiFiEventArgs args;
		args.Gateway=pparam->ip_info.gw.addr;
		args.Ip=pparam->ip_info.ip.addr;
		args.Subnet=pparam->ip_info.netmask.addr;
		hcon->Notify(WiFiEvent::StationConnected, &args);
		break;
		}
	default:
		{
		break;
		}
	}
}

VOID WiFiCallback(VOID* parg, esp_event_base_t pevent, INT id, VOID* pdata)
{
auto hcon=WiFiConnection::Current;
if(!hcon)
	return;
switch(id)
	{
	case WIFI_EVENT_AP_STACONNECTED:
		{
		hcon->Notify(WiFiEvent::ClientConnected, nullptr);
		break;
		}
	case WIFI_EVENT_AP_STADISCONNECTED:
		{
		hcon->Notify(WiFiEvent::ClientDisconnected, nullptr);
		break;
		}
	case WIFI_EVENT_STA_DISCONNECTED:
		{
		hcon->Notify(WiFiEvent::StationDisconnected, nullptr);
		break;
		}
	case WIFI_EVENT_STA_START:
		{
		hcon->Notify(WiFiEvent::StationStarted, nullptr);
		break;
		}
	default:
		{
		break;
		}
	}
}


//========
// Common
//========

VOID WiFiAccessPointClose()
{
esp_netif_dhcps_stop(pAccessPoint);
}

VOID WiFiAccessPointConfig(Handle<String> hnetwork, Handle<String> hpassword, UINT uip, UINT ugateway, UINT usubnet)
{
wifi_config_t conf;
ZeroMemory(&conf, sizeof(wifi_config_t));
conf.ap.authmode=hpassword? WIFI_AUTH_WPA2_PSK: WIFI_AUTH_OPEN;
conf.ap.beacon_interval=100;
conf.ap.channel=1;
conf.ap.max_connection=4;
if(hnetwork)
	conf.ap.ssid_len=StringCopy((LPSTR)conf.ap.ssid, 32, hnetwork->Begin());
if(hpassword)
	StringCopy((LPSTR)conf.ap.password, 64, hpassword->Begin());
if(esp_wifi_set_config(ESP_IF_WIFI_AP, &conf)!=ESP_OK)
	{
	DebugPrint("esp_wifi_set_config(AP) failed\n");
	return;
	}
esp_netif_dhcps_stop(pAccessPoint);
esp_netif_ip_info_t info;
info.ip.addr=uip;
info.gw.addr=ugateway;
info.netmask.addr=usubnet;
if(esp_netif_set_ip_info(pAccessPoint, &info)!=ESP_OK)
	{
	DebugPrint("esp_netif_set_ip_info(AP) failed\n");
	return;
	}
if(esp_netif_dhcps_start(pAccessPoint)!=ESP_OK)
	{
	DebugPrint("esp_netif_dhcps_start(AP) failed\n");
	return;
	}
}

VOID WiFiClose()
{
if(esp_wifi_disconnect()!=ESP_OK)
	{
	DebugPrint("esp_wifi_disconnect() failed\n");
	}
if(esp_wifi_stop()!=ESP_OK)
	{
	DebugPrint("esp_wifi_stop() failed\n");
	}
}

VOID WiFiInit()
{
esp_netif_init();
if(esp_event_loop_create_default()!=ESP_OK)
	{
	DebugPrint("esp_event_loop_create_default() failed\n");
	return;
	}
if(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, IpCallback, nullptr)!=ESP_OK)
	{
	DebugPrint("esp_event_handler_register(IP) failed\n");
	return;
	}
if(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, WiFiCallback, nullptr)!=ESP_OK)
	{
	DebugPrint("esp_event_handler_register(WIFI) failed\n");
	return;
	}
pAccessPoint=esp_netif_create_default_wifi_ap();
pStation=esp_netif_create_default_wifi_sta();
wifi_init_config_t wificfg;
ZeroMemory(&wificfg, sizeof(wifi_init_config_t));
wificfg.event_handler=&esp_event_send_internal;
wificfg.osi_funcs=&g_wifi_osi_funcs;
wificfg.wpa_crypto_funcs=g_wifi_default_wpa_crypto_funcs;
wificfg.static_rx_buf_num=CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM;
wificfg.dynamic_rx_buf_num=CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM;
wificfg.tx_buf_type=CONFIG_ESP32_WIFI_TX_BUFFER_TYPE;
wificfg.static_tx_buf_num=WIFI_STATIC_TX_BUFFER_NUM;
wificfg.dynamic_tx_buf_num=WIFI_DYNAMIC_TX_BUFFER_NUM;
wificfg.csi_enable=WIFI_CSI_ENABLED;
wificfg.ampdu_rx_enable=WIFI_AMPDU_RX_ENABLED;
wificfg.ampdu_tx_enable=WIFI_AMPDU_TX_ENABLED;
wificfg.nvs_enable=WIFI_NVS_ENABLED;
wificfg.nano_enable=WIFI_NANO_FORMAT_ENABLED;
wificfg.tx_ba_win=WIFI_DEFAULT_TX_BA_WIN;
wificfg.rx_ba_win=WIFI_DEFAULT_RX_BA_WIN;
wificfg.wifi_task_core_id=WIFI_TASK_CORE_ID;
wificfg.beacon_max_len=WIFI_SOFTAP_BEACON_MAX_LEN;
wificfg.mgmt_sbuf_num=CONFIG_ESP32_WIFI_MGMT_SBUF_NUM;
wificfg.magic=WIFI_INIT_CONFIG_MAGIC;
if(esp_wifi_init(&wificfg)!=ESP_OK)
	{
	DebugPrint("esp_wifi_init() failed\n");
	return;
	}
}

VOID WiFiStationClose()
{
esp_netif_dhcpc_stop(pStation);
}

VOID WiFiStationConfig(Handle<String> hnetwork, Handle<String> hpassword)
{
wifi_config_t conf;
ZeroMemory(&conf, sizeof(wifi_config_t));
conf.sta.channel=WiFiChannel;
if(hnetwork)
	StringCopy((LPSTR)conf.sta.ssid, 32, hnetwork->Begin());
if(hpassword)
	StringCopy((LPSTR)conf.sta.password, 64, hpassword->Begin());
if(esp_wifi_set_config(ESP_IF_WIFI_STA, &conf)!=ESP_OK)
	{
	DebugPrint("esp_wifi_set_config(STA) failed\n");
	return;
	}
if(esp_netif_dhcpc_start(pStation)!=ESP_OK)
	{
	DebugPrint("esp_netif_dhcpc_start(STA) failed\n");
	return;
	}
}

VOID WiFiStationSetHostName(Handle<String> hhostname)
{
if(esp_netif_set_hostname(pStation, hhostname? hhostname->Begin(): nullptr)!=ESP_OK)
	{
	DebugPrint("esp_netif_set_hostname(STA) failed\n");
	}
}

}}