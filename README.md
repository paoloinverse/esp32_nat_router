
# ESP32_nat_router with pseudo-mesh
# Introduction (I recommend you read at least *this* part and the last part at the end of this README)

Hello, 
this is a fork of Martin-Ger's awesome ESP32 NAT Router code, see the original code at https://github.com/martin-ger/esp32_nat_router

As it happens, I needed to build a hierarchical network of cascaded ESP32 nat-routers with failover and self-healing capabilities, of course each ESP32 needed to have its own IP addressing space so I just modified Martin's project
and added exactly this: 

1) Ability to set multiple STA configurations, numbered from 0 to 15, if the ESP32 disconnects or fails to connect to the default remote AP number 0, then the next AP saved to flash is used. The STA configurations allow up to 16 different AP that can be cycled through in a round robin fashion. This process lets one build layered, cascaded networks of ESP32 NAT/Routers with self-healing capabilities that basically come for free during the failover process. Make sure that the devices cannot form routing loops, that would be VERY bad. You should rather try to build a layered, tree-like topology. There can be multiple AP at the vertices, that are connected to the Internet. If one fails, the other takes over!

2) Ability to assign the soft AP a different addressing space to each ESP32 device, forget being limited to the default 192.168.4.0/24 network. This is needed as consequence of being able to cascade EAP32 nat routers anyways. You may set whatever you want as long as you accept the default 255.255.255.0 hardwired netmask. I don't see this as a limitation, the ESP32 wouldn't be able to handle that many clients (254) anyways. By the way the netmask is hardcoded, feel free to change it to whatever, if you want. 

3) Added a serial console command, list_alternate , that allows you to quickly view the list of configured failover STA's. That might save the day. 

4) Added configurable heartbeats and minimal log messages. Whenever a configuration change happens, a wifi event happens or any command is received from serial, or just every 60 seconds, a udp message is sent to two redundant remote servers. The "heartbeat" message transports a minimalist layer 5 protocol over udp and basically signals the device (its hostname) is alive and working, plus access to configuration UIs and some errors/events generate "log" messages with some useful information. In short, this layer 5 protocol is simply a text string of 8 (eight) comma separated variables. Each variable can be up to 64 bytes long, although I suggest keeping the whole UDP payload under 128 bytes in case we want to encapsulate it in an ESP-NOW frame in the future. I added a timed task to periodically send out the heartbeat packets. This comes in handy if you need to remotely monitor the devices, especially in unmanned locations. The IP addresses of the remote servers, and the UDP ports are configurable via the embedded http server, the default configuration is hardcoded but any changes are saved in the NVS. You may want to compile your own firmware with your own defaults. 

In order to compile this code:
make sure to use Martin's modified LWIP library with NAT together with the ESP-IDF framework, at least version 4, you may find the instructions at the end of this README. Notice the code is currently (2020-03-31) using tcpip_adapter_init() and  esp_event_loop_init() which are now deprecated from ESP-IDF 4.1 and later. I'm in the process of migrating to esp_netif_init(); a feat that takes extensive modifications to the core code. If you see warnings but use ESP-IDF 4.0 or 4.2 (recommended), don't worry. For the moment.

Migrating to long term supported functions is in order AND will happen.


# ESP32 NAT Router

This is a firmware to use the ESP32 as WiFi NAT router. It can be used as
- Simple range extender for an existing WiFi network
- Setting up an additional WiFi network with different SSID/password for guests or IOT devices

It can achieve a bandwidth of more than 15mbps.

The code is based on the [Console Component](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/console.html#console) and the [esp-idf-nat-example](https://github.com/jonask1337/esp-idf-nat-example). 

## First Boot
After first boot the ESP32 NAT Router will offer a WiFi network with an open AP and the ssid "ESP32_NAT_Router". Configuration can either be done via a simple web interface or via the serial console. 

## Web Config Interface
The web interface allows for the configuration of all parameters. Connect you PC or smartphone to the WiFi SSID "ESP32_NAT_Router" and point your browser to "http://192.168.5.1" (my prebuilt binary, or built from the current code) or "http://192.168.4.1" (default address) whichever works first. This page should appear:

<img src="https://raw.githubusercontent.com/paoloinverse/esp32_nat_router/master/ESP32_NAT_UI.JPG">

First enter the appropriate values for the uplink WiFi network, the "STA Settings". Leave password blank for open networks. Click "Connect". The ESP32 reboots and will connect to your WiFi router.

Now you can reconnect and reload the page and change the "Soft AP Settings". Click "Set" and again the ESP32 reboots. Now it is ready for forwarding traffic over the newly configured Soft AP. Be aware that these changes also affect the config interface, i.e. to do further configuration, connect to the ESP32 through one of the newly configured WiFi networks.

If you want to enter a '+' in the web interface you have to use HTTP-style hex encoding like "Mine%2bYours". This will result in a string "Mine+Yours". With this hex encoding you can enter any byte value you like, except for 0 (for C-internal reasons).

It you want to disable the web interface (e.g. for security reasons), go to the CLI and enter:
```
nvs_namespace esp32_nat
nvs_set lock str -v 1
```
After restart, no webserver is started any more. You can only re-enable it with:
```
nvs_namespace esp32_nat
nvs_set lock str -v 0
```
If you made a mistake and have lost all contact with the ESP you can still use the serial console to reconfigure it. All parameter settings are stored in NVS (non volatile storage), which is *not* erased by simple re-flashing the binaries. If you want to wipe it out, use "esptool.py -p /dev/ttyUSB0 erase_flash".

# Command Line Interface

For configuration you have to use a serial console (Putty or GtkTerm with 115200 bps).
Use the "set_sta" and the "set_ap" command to configure the WiFi settings. Changes are stored persistently in NVS and are applied after next restart. Use "show" to display the current config.

Enter the `help` command get a full list of all available commands.

If you want to enter non-ASCII or special characters (incl. ' ') you can use HTTP-style hex encoding (e.g. "My%20AccessPoint" results in a string "My AccessPoint").

## Flashing the prebuild Binaries
Install [esptool](https://github.com/espressif/esptool), go to the project directory, and enter:
```
esptool.py --chip esp32 --port /dev/ttyUSB0 \
--baud 115200 --before default_reset --after hard_reset write_flash \
-z --flash_mode dio --flash_freq 40m --flash_size detect \
0x1000 build/bootloader/bootloader.bin \
0x10000 build/esp32_nat_router.bin \
0x8000 build/partitions_example.bin
```

As an alternative you might use [Espressif's Flash Download Tools](https://www.espressif.com/en/products/hardware/esp32/resources) with the parameters given in the figure below (thanks to mahesh2000):

![image](https://raw.githubusercontent.com/martin-ger/esp32_nat_router/master/FlasherUI.jpg)

## Building the Binaries
The following are the steps required to compile this project:

### Step 1 - Setup ESP-IDF
Download and setup the ESP-IDF.

### Step 2 - Get the lwIP library with NAT
Download the repository of the NAT lwIP library using the follwing command:

`git clone https://github.com/martin-ger/esp-lwip.git`

Note: It is important to clone the repository. If it is only downloaded it will be replaced by the orginal lwIP library during the build.

### Step 3 - Replace original ESP-IDF lwIP library with NAT lwIP library
1. Go to the folder where the ESP-IDF is installed.
2. Rename or delete the *esp-idf/component/lwip/lwip* directory.
3. Move the lwIP library with NAT repository folder from Step 2 to *esp-idf/component/lwip/*
4. Rename the lwIP library with NAT repository folder from *esp-lwip* to *lwip*.

### Step 4 - Build and flash the esp-idf-nat-example project
1. Configure and set the option "Enable copy between Layer2 and Layer3 packets" in the ESP-IDF project configuration.
    1. In the project directory run `make menuconfig` (or `idf.py menuconfig` for cmake).
    2. Go to *Component config -> LWIP* and set "Enable copy between Layer2 and Layer3 packets" option.
2. Build the project and flash it to the ESP32.

A detailed instruction on how to build, configure and flash a ESP-IDF project can also be found the official ESP-IDF guide.

### DNS
By Default the DNS-Server which is offerd to clients connecting to the ESP32 AP is set to 8.8.8.8.
Replace the value of the *MY_DNS_IP_ADDR* with your desired DNS-Server IP address (in hex) if you want to use a different one.

## Troubleshooting

### Line Endings

The line endings in the Console Example are configured to match particular serial monitors. Therefore, if the following log output appears, consider using a different serial monitor (e.g. Putty for Windows or GtkTerm on Linux) or modify the example's UART configuration.

```
This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
Your terminal application does not support escape sequences.
Line editing and history features are disabled.
On Windows, try using Putty instead.
esp32>
```
