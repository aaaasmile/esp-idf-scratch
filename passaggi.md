# wifi client con Idf
Per avere idf in Visual Code, che va lanciato in PowerShell con start_code.ps1, devo lanciare nel terminal:

    C:\Espressif\tools\Microsoft.v6.0.1.PowerShell_profile.ps1

per settare il target

    idf.py set-target esp32c5
    
Deepseek consiglia:
    idf.py --preview set-target esp32c5

per compilare il progetto:

    idf.py build

per sapere la porta:

    mode

per il flash:

    idf.py -p COM6 flash monitor

L'esempio col quale sono partito è:

    D:\Arduino\esp32-idf\idf\.espressif\v6.0.1\esp-idf\examples\wifi\getting_started\station\main\station_example_main.c
L'esempio che a me interessa è quello che si collega alla WLAN come client e non so per quale
oscura ragione l'esempio è chiamato station.