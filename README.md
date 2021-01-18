# Care-support-system

Care-support-systemは、Appleの提供するiBeaconを使用しています。  

* Peripheral : タッチセンサ、近接センサ、環境センサを取り付けたPSoC  
* Central : サーバーの役割のラズパイ  

## 開発環境  
* Peripheral:PSoC4BLE(CYBLE-022001-00)  
* Central:Raspberry Pi3  
* Description:Raspian GNU/Linux 10 (buster)  
* Grafana v6.5.2  
* InfluxDB v1.6.4-1  

### 使用したセンサ
IRsensor:VL6180x https://www.switch-science.com/catalog/2518/  
Environmental sensor:BME280 https://www.switch-science.com/catalog/2236/  

## システム全体図
![Care-support-system system-overview](https://user-images.githubusercontent.com/66234583/105314958-f7f9d480-5c01-11eb-85c2-d883a3017011.png)

## 文責
name: Takumi Yamanaka (山中拓己)  
mail: y.takumi4@gmail.com  