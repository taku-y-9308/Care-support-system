# Care-support-system

Care-support-system uses the Apple provided iBeacon.

* Peripheral : A PSoC attached to a tracheostomy tube
* Central: A Raspberrypi in the role of a server

----------------------------------------------------------------

Care-support-systemは、Appleの提供するiBeaconを使用しています。

* Peripheral : 気管切開チューブに取り付けたPSoC
* Central : サーバーの役割のラズパイ

## Development Environment
Slave:PSoC4BLE(CYBLE-022001-00)  
Master:Raspberry Pi3  
OS:Raspberry Pi OS Lite  
Grafana v6.5.2  
InfluxDB v1.6.4-1  

### Used sensor
IRsensor:VL6180x https://www.switch-science.com/catalog/2518/  
Environmental sensor:BME280 https://www.switch-science.com/catalog/2236/  

## System Overview
![Care-support-system system-overview](https://user-images.githubusercontent.com/66234583/105314958-f7f9d480-5c01-11eb-85c2-d883a3017011.png)

However, as for OpenWeather, we are stopping it now because the process becomes too heavy.  
We plan to introduce it by sending requests less frequently.

ただ、OpenWeatherに関しては、処理が重くなってしまうので、ただいま停止中です。  
リクエストを送る頻度を落として導入予定です。

## Responsibility
name: Takumi Yamanaka (山中拓己)  
mail: y.takumi4@gmail.com