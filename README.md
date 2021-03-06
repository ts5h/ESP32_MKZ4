# ESP32 MKZ4
Customized MKZ4 Firmware for ESP32 (**NOT** for ESP8266).  
Based on `cerevo/MKZ4`.  
https://github.com/cerevo/MKZ4  

### TAMIYA Wild Mini 4WD
https://www.ebay.com/itm/113952018901

### MKZ4 Customize Kit
https://cerevo.shop-pro.jp/?pid=104131889  
(Official Shop Link, but may be only in Japan...)   


![from_right_4_3](https://user-images.githubusercontent.com/1344010/174665697-8dc8e9f1-966f-4778-bd89-090ed0802f1e.png)

*My machine works with these codes for now, but I don't expect them to work universally for all DIY products, so I recommend customizing them.


## Schematic
See `schematic/schematic.pdf`


## ESP32-Arduino / Installing using Arduino IDE

https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html


## Dependencies

- Servo Library for ESP32
https://www.arduino.cc/reference/en/libraries/esp32servo/
https://github.com/madhephaestus/ESP32Servo


## Ex) VS Code / c_cpp_properties.json

Add ESP32 libraries path to "includePath" like below:

```.vscode/c_cpp_properties.json
"includePath": [
    "C:\\Users\\at4\\AppData\\Local\\Arduino15\\packages\\esp32\\hardware\\esp32\\2.0.2\\libraries",
    "${workspaceFolder}/**",
    "..\\libraries"
],
```


## Licence

[BSD 3-Clause License](https://opensource.org/licenses/BSD-3-Clause)
