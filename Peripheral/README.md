# Peripheral

## 使用したセンサのデータシート
VL6180x(for IRsensor):https://www.pololu.com/file/0J961/VL6180X.pdf  
BME280(for Environmental sensor):https://akizukidenshi.com/download/ds/bosch/BST-BME280_DS001-10.pdf  

## パケットの割り当て
Appleが提供するiBeaconを使ってサーバーへと情報を送信しています。  

![ibeacon](https://user-images.githubusercontent.com/66234583/105325896-72c9ec00-5c10-11eb-9e2a-316bd700510c.png)

iBeaconのパケットの中で、開発者が編集できる部分は、"Major領域"と"Minor領域"の２つがあります。  
この２つの領域を以下のように割り当てています。  

![ibeacon_major_minor](https://user-images.githubusercontent.com/66234583/105328216-2a5ffd80-5c13-11eb-9ea0-767c4ec8fcf5.png)

一番上の数字(ex 25~28):iBeaconパケットを配列に割り当てた時の配列番号  
一番下の数字(ex 5,8):割り当てているビット数  

## 表示できる範囲
iBeaconを使って送れるデータの大きさには制限があるので、範囲に制限をかけています。  

* temp:10℃~42℃  
* humi:20%~84% (However, 2% increments)  
* Pressure:900hPa~1156hPa  

