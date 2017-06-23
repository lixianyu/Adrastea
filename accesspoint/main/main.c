#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include <lwip/sockets.h>

#define AP_SSID "esp32-ap"
#define AP_PSK "MakerModule"

// set AP CONFIG values
#ifdef CONFIG_AP_HIDE_SSID
#define CONFIG_AP_SSID_HIDDEN 1
#else
#define CONFIG_AP_SSID_HIDDEN 0
#endif
#ifdef CONFIG_WIFI_AUTH_OPEN
#define CONFIG_AP_AUTHMODE WIFI_AUTH_OPEN
#endif
#ifdef CONFIG_WIFI_AUTH_WEP
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WEP
#endif
#ifdef CONFIG_WIFI_AUTH_WPA_PSK
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WPA_PSK
#endif
#ifdef CONFIG_WIFI_AUTH_WPA2_PSK
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WPA2_PSK
#endif
#ifdef CONFIG_WIFI_AUTH_WPA_WPA2_PSK
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WPA_WPA2_PSK
#endif
#ifdef CONFIG_WIFI_AUTH_WPA2_ENTERPRISE
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WPA2_ENTERPRISE
#endif


#define PORT_NUMBER 4567
#define BLINK_GPIO GPIO_NUM_5

static char tag[] = "socket_server";


// Event group
static EventGroupHandle_t event_group;
const int STA_CONNECTED_BIT = BIT0;
const int STA_DISCONNECTED_BIT = BIT1;

// AP event handler
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {

    case SYSTEM_EVENT_AP_START:
        printf("Access point started\n");
        break;

    case SYSTEM_EVENT_AP_STACONNECTED:
        xEventGroupSetBits(event_group, STA_CONNECTED_BIT);
        break;

    case SYSTEM_EVENT_AP_STADISCONNECTED:
        xEventGroupSetBits(event_group, STA_DISCONNECTED_BIT);
        break;

    default:
        break;
    }

    return ESP_OK;
}

// print the list of connected stations
void printStationList()
{
    printf(" Connected stations:\n");
    printf("--------------------------------------------------\n");

    wifi_sta_list_t wifi_sta_list;
    tcpip_adapter_sta_list_t adapter_sta_list;

    memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
    memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));

    ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&wifi_sta_list));
    ESP_ERROR_CHECK(
        tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list));

    for (int i = 0; i < adapter_sta_list.num; i++)
    {

        tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
        printf("%d - mac: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x - IP: %s\n", i + 1,
               station.mac[0], station.mac[1], station.mac[2], station.mac[3],
               station.mac[4], station.mac[5], ip4addr_ntoa(&(station.ip)));
    }

    printf("\n");
}

// Monitor task, receive Wifi AP events
void monitor_task(void *pvParameter)
{
    while (1)
    {

        EventBits_t staBits = xEventGroupWaitBits(event_group,
                              STA_CONNECTED_BIT | STA_DISCONNECTED_BIT, pdTRUE, pdFALSE,
                              portMAX_DELAY);
        if ((staBits & STA_CONNECTED_BIT) != 0)
        {
            printf("New station connected\n\n");
        }
        else
        {
            printf("A station disconnected\n\n");
        }

    }
}

//this task establish a TCP connection and receive data from TCP
static void server_task(void *pvParameters)
{

    struct sockaddr_in clientAddress;
    struct sockaddr_in serverAddress;


    // Create a socket that we will listen upon.
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        ESP_LOGE(tag, "socket: %d %s", sock, strerror(errno));
        goto END;
    }

    // Bind our server socket to a port.
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT_NUMBER);
    int rc  = bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (rc < 0)
    {
        ESP_LOGE(tag, "bind: %d %s", rc, strerror(errno));
        goto END;
    }

    // Flag the socket as listening for new connections.
    rc = listen(sock, 5); //5 concurrent connections
    if (rc < 0)
    {
        ESP_LOGE(tag, "listen: %d %s", rc, strerror(errno));
        goto END;
    }


    while (1)
    {
        // Listen for a new client connection.
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSock = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLength);
        //              clientSock = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSock < 0)
        {
            ESP_LOGE(tag, "accept: %d %s", clientSock, strerror(errno));
            printf("accept: %d %s", clientSock, strerror(errno));
            goto END;
        }

        // We now have a new client ...
        int total = 4; //bytes 10*1024 // we send 4 chars for now
        int sizeUsed = 0;
        char *data = malloc(total);
        char buffer[256];
        int n;

        bzero(buffer, 256);
        n = read(clientSock, buffer, 255);

        if (n < 0) printf("ERROR reading from socket");
        printf("Here is the message: %s\n", buffer);

        //send back the data
        int sock = clientSock;
        char msg[] = "";


        if (strncmp(buffer, "ONON", 2) == 0)
        {
            strcpy(msg, "LED is now on");
            gpio_set_level(BLINK_GPIO, 1);
        }
        else if (strncmp(buffer, "OOFF", 3) == 0)
        {
            strcpy(msg, "LED is now off");
            gpio_set_level(BLINK_GPIO, 0);
        }
        else
        {
            strcpy(msg, "Got dummy text");
        }


        //socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        send(sock, msg, strlen(msg), 0);
        //ESP_LOGD(tag, "send: rc: %d", rc);

        //close outgoing socket
        //rc = close(sock);
        //ESP_LOGD(tag, "close: rc: %d", rc);

        //close incoming socket
        free(data);
        close(clientSock);
    }
END:
    vTaskDelete(NULL);


    //	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //	struct sockaddr_in serverAddress;
    //	serverAddress.sin_family = AF_INET;
    //	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    //	serverAddress.sin_port = htons(4567);
    //	bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    //
    //	listen(sock, backlog);
    //
    //	struct sockaddr_in clientAddress;
    //	socklen_t clientAddressLength = sizeof(clientAddress);
    //	int clientSock = accept(sock, (struct sockaddr *)&clientAddress,
    //	&clientAddressLength);
    //
    //	struct sockaddr_in serverAddress;
    //	serverAddress.sin_family = AF_INET;
    //	inet_pton(AF_INET, "192.168.1.200", &serverAddress.sin_addr.s_addr);
    //	serverAddress.sin_port = htons(9999);
    //	int rc = connect(sock, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in));





}

// Station list task, print station list every 10 seconds
void station_list_task(void *pvParameter)
{
    while (1)
    {

        printStationList();
        vTaskDelay(10000 / portTICK_RATE_MS);
    }
}

// Main application
void app_main()
{

    // disable the default wifi logging
    esp_log_level_set("wifi", ESP_LOG_NONE);

    // create the event group to handle wifi events
    event_group = xEventGroupCreate();

    // initialize the tcp stack
    tcpip_adapter_init();

    // stop DHCP server
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));

    // assign a static IP to the network interface
    tcpip_adapter_ip_info_t info;
    memset(&info, 0, sizeof(info));
    IP4_ADDR(&info.ip, 192, 168, 10, 1);
    IP4_ADDR(&info.gw, 192, 168, 10, 1);
    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
    ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));

    // start the DHCP server
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));

    // initialize the wifi event handler
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    // initialize the wifi stack in AccessPoint mode with config in RAM
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT()
                                          ;
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    // configure the wifi connection and start the interface
    wifi_config_t ap_config = { .ap = {
            //            .ssid = CONFIG_AP_SSID,
            //            .password = CONFIG_AP_PASSWORD,
            //			.ssid_len = 0,
            //			.channel = CONFIG_AP_CHANNEL,
            //			.authmode = CONFIG_AP_AUTHMODE,
            //			.ssid_hidden = CONFIG_AP_SSID_HIDDEN,
            //			.max_connection = CONFIG_AP_MAX_CONNECTIONS,
            //			.beacon_interval = CONFIG_AP_BEACON_INTERVAL,
            .ssid = AP_SSID, .password = AP_PSK, .ssid_len = 0, .channel = 0,
            .authmode = CONFIG_AP_AUTHMODE,
            .ssid_hidden = CONFIG_AP_SSID_HIDDEN, .max_connection = 4,
            .beacon_interval = 100,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

    // start the wifi interface
    ESP_ERROR_CHECK(esp_wifi_start());
    printf("Starting access point, SSID=%s\n", AP_SSID);

    // start the main task
    xTaskCreate(&monitor_task, "monitor_task", 2048, NULL, 5, NULL);
    //	xTaskCreate(&station_list_task, "station_list_task", 2048, NULL, 5, NULL);
    xTaskCreate(&server_task, "server_task", 2048, NULL, 5, NULL);
}
