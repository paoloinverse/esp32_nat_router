#define CONFIG_PAGE "\
<html>\
<head></head>\
<meta name='viewport' content='width=device-width, initial-scale=1'>\
<body>\
<h1>ESP32 NAT Router Config</h1>\
<div id='config'>\
<script>\
if (window.location.search.substr(1) != '')\
{\
document.getElementById('config').display = 'none';\
document.body.innerHTML ='<h1>ESP32 NAT Router Config</h1>The new settings have been sent to the device...';\
setTimeout(\"location.href = '/'\",10000);\
}\
</script>\
<h2>STA Settings</h2>\
<form action='' method='GET'>\
<table>\
<tr>\
<td>SSID:</td>\
<td><input type='text' name='ssid' value='%s'/></td>\
</tr>\
<tr>\
<td>Password:</td>\
<td><input type='text' name='password' value='%s'/></td>\
</tr>\
<td><input type='number' name='stanum' value='0' min='0' max='15'/>STA number, any number >0 and <15 is used as a failover in case the number 0 disconnects. </td>\
<td>Remember to fill in from number 0, and DO NOT LEAVE GAPS between position numbers</td>\
<tr>\
<td></td>\
<td><input type='submit' value='Connect'/></td>\
</tr>\
\
</table>\
</form>\
\
<h2>AP Settings</h2>\
<form action='' method='GET'>\
<table>\
<tr>\
<td>SSID:</td>\
<td><input type='text' name='ap_ssid' value='%s'/></td>\
</tr>\
<tr>\
<td>Password:</td>\
<td><input type='text' name='ap_password' value='%s'/></td>\
</tr>\
</tr>\
<tr>\
<td>IP address:</td>\
<td><input type='number' name='ip_a' value='192'/></td>\
.<td><input type='number' name='ip_b' value='168'/></td>\
.<td><input type='number' name='ip_c' value='5'/></td>\
.<td><input type='number' name='ip_d' value='1'/></td>\
</tr>\
<tr>\
<td></td>\
<td><input type='submit' value='Set' /></td>\
</tr>\
</table>\
<small>\
<i>Password: </i>less than 8 chars = open<br />\
</small>\
</form>\
\
\
<h2>UDP Heartbeat servers (Optional, configure two redundant servers and the destination port)</h2>\
<form action='' method='GET'>\
<table>\
<tr>\
<tr>\
<td>LFCP Server A (IP Address only, do not use FQDNs):</td>\
<td><input type='text' name='LFCPserverA' value='192.168.88.32'/></td>\
</tr>\
<tr>\
<td>LFCP Server B (IP Address only, do not use FQDNs):</td>\
<td><input type='text' name='LFCPserverB' value='192.168.88.33'/></td>\
</tr>\
<tr>\
<td>UDP Port number (default is 8443):</td>\
<td><input type='number' name='LFCPport' value='8443'/></td>\
</tr>\
</table>\
<small>\
<i>If in doubt, do not touch these settings</i>(they just send out harmless udp packets)<br />\
</small>\
<tr>\
<td></td>\
<td><input type='submit' value='Set' /></td>\
</tr>\
</form>\
<h2>Device Management</h2>\
<form action='' method='GET'>\
<table>\
<tr>\
<td>Reset Device:</td>\
<td><input type='submit' name='reset' value='Restart'/></td>\
</tr>\
</table>\
</form>\
</div>\
</body>\
</html>\
"

#define LOCK_PAGE "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n\
<html>\
<head></head>\
<meta name='viewport' content='width=device-width, initial-scale=1'>\
<body>\
<h1>ESP32 NAT Router Config</h1>\
<div id='config'>\
<script>\
if (window.location.search.substr(1) != '')\
{\
document.getElementById('config').display = 'none';\
document.body.innerHTML ='<h1>ESP WiFi NAT Router Config</h1>Unlock request has been sent to the device...';\
setTimeout(\"location.href = '/'\",1000);\
}\
</script>\
<h2>Config Locked</h2>\
<form autocomplete='off' action='' method='GET'>\
<table>\
<tr>\
<td>Password:</td>\
<td><input type='password' name='unlock_password'/></td>\
</tr>\
<tr>\
<td></td>\
<td><input type='submit' value='Unlock'/></td>\
</tr>\
\
</table>\
<small>\
<i>Default: STA password to unlock<br />\
</small>\
</form>\
</div>\
</body>\
</html>\
"
