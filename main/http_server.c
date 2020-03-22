/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_timer.h>
#include <sys/param.h>
//#include "nvs_flash.h"
#include "esp_netif.h"
//#include "esp_eth.h"
//#include "protocol_examples_common.h"

#include <esp_http_server.h>

#include "pages.h"
#include "esp32_nat_router.h"

// #include <stdlib.h> // need the atoi function


static const char *TAG = "HTTPServer";

esp_timer_handle_t restart_timer;

static void restart_timer_callback(void* arg)
{
    ESP_LOGI(TAG, "Restartting now...");
    esp_restart();
}

esp_timer_create_args_t restart_timer_args = {
        .callback = &restart_timer_callback,
        /* argument specified here will be passed to timer callback function */
        .arg = (void*) 0,
        .name = "restart_timer"
};

/* An HTTP GET handler */
static esp_err_t index_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            if (strcmp(buf, "reset=Restart") == 0) {
                esp_timer_start_once(restart_timer, 500000);
            }
            char param1[64];
            char param2[64];
	    char param3[64];
	    char param4[64];
	    char param5[64];
	    char param6[64];
	    int ipaint = 0;
	    int ipbint = 0;
	    int ipcint = 0;
	    int ipdint = 0;
	    int ipabcd[4];

	    ipabcd[0] = 0;
	    ipabcd[1] = 0;
	    ipabcd[2] = 0;
	    ipabcd[3] = 0;
	    
	    int STApos = 0;


            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "ssid", param1, sizeof(param1)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => ssid=%s", param1);
                preprocess_string(param1);
                if (httpd_query_key_value(buf, "password", param2, sizeof(param2)) == ESP_OK) {
                    ESP_LOGI(TAG, "Found URL query parameter => password=%s", param2);
		

                    preprocess_string(param2);
                    int argc = 3;
                    char *argv[3];
                    argv[0] = "set_sta";
                    argv[1] = param1;
                    argv[2] = param2;
                    set_sta(argc, argv);


		    if (httpd_query_key_value(buf, "stanum", param3, sizeof(param3)) == ESP_OK) {
		    	ESP_LOGI(TAG, "Found URL query parameter => stanum=%s", param3);
			STApos = atoi(param3);
			set_staAlt(argc, argv, STApos);
		    }
		    
                    esp_timer_start_once(restart_timer, 500000);
                }
            }
            if (httpd_query_key_value(buf, "ap_ssid", param1, sizeof(param1)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => ap_ssid=%s", param1);
                preprocess_string(param1);
                if (httpd_query_key_value(buf, "ap_password", param2, sizeof(param2)) == ESP_OK) {
                    ESP_LOGI(TAG, "Found URL query parameter => ap_password=%s", param2);
		    // SoftAP IP subsection
		    if (httpd_query_key_value(buf, "ip_a", param3, sizeof(param3)) == ESP_OK) {
		    	ESP_LOGI(TAG, "Found URL query parameter => ip_a=%s", param3);
			if (httpd_query_key_value(buf, "ip_b", param4, sizeof(param4)) == ESP_OK) {	
		    		ESP_LOGI(TAG, "Found URL query parameter => ip_b=%s", param4);
				if (httpd_query_key_value(buf, "ip_c", param5, sizeof(param5)) == ESP_OK) {
		    			ESP_LOGI(TAG, "Found URL query parameter => ip_c=%s", param5);
					if (httpd_query_key_value(buf, "ip_d", param6, sizeof(param6)) == ESP_OK) {
		    				ESP_LOGI(TAG, "Found URL query parameter => ip_d=%s", param6);
						// saving the IP address
						ipabcd[0] = atoi(param3);
						ipabcd[1] = atoi(param4);
						ipabcd[2] = atoi(param5);
						ipabcd[3] = atoi(param6);
						printf("http_server received ip data: %d %d %d %d",ipabcd[0],ipabcd[1],ipabcd[2],ipabcd[3]);
                    				save_ip_addr(ipabcd);

					}
				}
			}
		    }
		    
		    

                    preprocess_string(param2);
                    int argc = 3;
                    char *argv[3];
                    argv[0] = "set_ap";
                    argv[1] = param1;
                    argv[2] = param2;
		    
                    // saving the AP config
		    set_ap(argc, argv);
                    esp_timer_start_once(restart_timer, 500000);
                }
            }
        }
        free(buf);
    }

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    return ESP_OK;
}

static httpd_uri_t indexp = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_get_handler,
};

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Page not found");
    return ESP_FAIL;
}

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    const char *config_page_template = CONFIG_PAGE;
    char *config_page = malloc(strlen(config_page_template)+512);
    sprintf(config_page, config_page_template, ssid, passwd, ap_ssid, ap_passwd);
    indexp.user_ctx = config_page;

    esp_timer_create(&restart_timer_args, &restart_timer);

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &indexp);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}
