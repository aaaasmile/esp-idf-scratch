# wifi client con Idf
Per avere idf in Visual Code, che va lanciato in PowerShell con start_code.ps1, devo lanciare nel terminal:

    C:\Espressif\tools\Microsoft.v6.0.1.PowerShell_profile.ps1

per settare il target

    idf.py set-target esp32c5
    
Deepseek consiglia:
    idf.py --preview set-target esp32c5

per compilare il progetto:

    idf.py build

per sapere la porta (funziona in PowerShell standalone, non in Terminal con Idf):

    mode

per il flash:

    idf.py -p COM7 flash monitor

L'esempio col quale sono partito è:

    D:\Arduino\esp32-idf\idf\.espressif\v6.0.1\esp-idf\examples\wifi\getting_started\station\main\station_example_main.c
L'esempio che a me interessa è quello che si collega alla WLAN come client e non so per quale
oscura ragione l'esempio è chiamato station. In ogni modo anche in https://wiki.seeedstudio.com/xiao_esp32c5_with_platformio/
la modalità client è chiamata station.

### c_cpp_properties.json
Sono riuscito a crearlo usando DeepSeek e la versione che avevo usato per AndroSolitario.
Il compiler è la sezione nuova così anche compileCommands

### configurare WiFi

    idf.py menuconfig
Poi si va in Component Config -> WiFi
Però non ho settato nulla. Invece ho definito 

    #define CONFIG_ESP_STATION_EXAMPLE_WPA3_SAE_PWE_HUNT_AND_PECK 1
    #define CONFIG_ESP_WIFI_AUTH_WPA2_PSK 1
per avere WPA2_PSK. Come l'ho trovato? Ho compilato il file wifi_scan.c invece die wifi_client.c
così nel monitor ho visto le informazioni del mio SSID che m'interessa. 

## https nella request

Referenza il progetto su 

    D:\Arduino\esp32-idf\idf\.espressif\v6.0.1\esp-idf\examples\protocols\esp_http_client\main

Per una richiesta https mi server il CA root certificate che si può avere con:

    openssl s_client -showcerts -connect invido.it:443 </dev/null
