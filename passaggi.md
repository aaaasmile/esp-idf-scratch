# wifi client con Idf
In questo progetto ho una serie di files in c per testare le singole funzionalità che mi servono 
per replicare il progetto GasTempBME680 (sviluppato con PlatformIO) per il target esp32c5. 

Nota che per usare il controller esp32c5 devo programmare in esp-idf e non posso riciclare il progetto precedente per ESP8266.
Le funzionalità che ho testato qui sono:
- client wifi 5 Ghz
- https client post
- led on/of via GPIO

Per avere idf in Visual Code, che va lanciato in PowerShell con start_code.ps1, devo lanciare nel terminal:

    C:\Espressif\tools\Microsoft.v6.0.1.PowerShell_profile.ps1
Il comando sopra è fonadamentale in quanto il build, flash e monitor avviene tutto usando la command line.

Per settare il target

    idf.py set-target esp32c5
    
Deepseek consiglia:
    idf.py --preview set-target esp32c5

per compilare il progetto:

    idf.py build

per sapere la porta (funziona in PowerShell standalone, non in Terminal con Idf):

    mode

per il flash:

    idf.py -p COM7 flash monitor
Per interrompere il Monitor uso la sequenza: CTRL + T CTRL + X

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
Poi il trucco è configurare esp_http_client_config_t config  con:

    .transport_type = HTTP_TRANSPORT_OVER_SSL,
    .cert_pem = invido_it_root_cert_pem_start,
Questo riesce a lanciare un http post usanto il protocollo https.

Per controllare se i dati vengono ricevuti basta usare _iot_ su invido.it

## Schema de ESP32 C5


                                                             +---------------------------+
                                                             |                           |
+------------------+                                         |                   GPIO 28 +--------->  Led Red
|                  |                                         |                           |
|                  |  VCC                                    |                   GPIO 27 +--------->  Led Yellow
|                  +-----------------------------------------|  3v3                      |
|                  |  GND                                    |                           |
|                  +-----------------------------------------+  GND              GPIO 23 +--------->  Led Green
|     BME680       |  SCL                                    |                           |
|                  +-----------------------------------------+  D1                       |
|                  |                                         |                           |
|                  |  SDA                                    |                           |
|                  |                                         |             ESP32-C5      |
|                  +---------------------------------------->+                           |
+------------------+                                         |  D2                       |
                                                             |                  Adc1-ch3 +---------> Button
                                                             |                           |
                                                             +---------------------------+

### Led
Per testare l'accensione e lo spegnimento dei tre leds ho creato il file led_test.c.
Il problema principale che ho avuto è stato nell'effettuare il collegamento dei fili. Lo schema della parte superiore 
non è precisa. Mentre è la parte posteriore dove sono chiari i collegamenti dei pin.
Nel file esempio gpio_example_main.c risulta molto chiaro l'utilizzo della configurazione e delle funzioni da usare.


## Pulsante per il check dello stato
TODO
Il tasto per mostrare lo stato è collegato al pin A0 con una resistenza di 220Ohm. Il tasto
è collegato in diagonale e l'altro capo è 3.3V. Quando il tasto è premuto il valore di A0 va sale a 1024.
Quando non è premuto il valore letto è < 10. Ho visto 1 e 7 nel log.  

## Sensore aria e gas BME680
TODO

## Installazione ESP-IDF
Come prerequisito ho bisogno di Python. L'ho installato usando la home di Python, ma ho solo installato i binary senza
integrazione nel path per evitare i classici conflitti di versione. Così per avere python in Visual Code uso start_code.ps1.
In Visual Code installo l'extension esp-idf. Poi installo il framework edf-idf usando l'extension in Visual Code. 
È un po' il contrario anche perché l'integrazione in Visual Code (per esempio build, flash e monitor) come in PlatformIO
a me proprio non funziona. Dopo l'installazione ho fatto un disable dell'extension ed uso solo il Terminal di Visual Code
dopo aver lanciato il powershell 

    Microsoft.v6.0.1.PowerShell_profile.ps1