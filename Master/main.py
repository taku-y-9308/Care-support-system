import time
import datetime
import influxdb
import get_weather_info
from beacontools import BeaconScanner, IBeaconFilter, IBeaconAdvertisement

#Store information in database
#User not created
db = influxdb.InfluxDBClient(
    host='localhost',
    port=8086,
    database='vl6180'
)
def callback(bt_addr, rssi, packet, additional_info):
	#Treat "start" as a global variable
	global start
	IR=(int(additional_info['minor'])>>8)&0xFF
	Capsense=(additional_info['minor'])&0xFF
	temp_humi=(int(additional_info['major'])>>8)&0xFF
	press_cons=(int(additional_info['major'])&0xFF)
	temp=((temp_humi>>3)&0xFF)+10
	humi=(((temp_humi)&0x07)*2)+20
	press=((press_cons>>4)&0xF)*2+900
	current_consumption=((press_cons)&0xF)*2+1000
	#POST data
	data={}
	#value from sensors
	data['IRsensor']=IR
	data['Capsense']=Capsense
	data['temp']=temp
	data['humi']=humi
	data['pressure']=press
	data['current_consumption']=current_consumption
	data['RSSI']=rssi

	#Get weather information every 30 minutes
	if time.time()-start>1800:
		data['weather_info']=get_weather_info.weather_info()
		start=time.time()#Reset the 1800s count by substituting the current UNIX time

	dt_now=datetime.datetime.utcnow()
	print(dt_now,data)
	write_to_influxdb(data)
	data['weather_info']=0

def write_to_influxdb(data):
    json_body = [{
        'measurement': 'result6',
        'time': datetime.datetime.utcnow(),
        'fields': data
    }]
    db.write_points(json_body)

if __name__ == "__main__":
	start=time.time()#Get the current UNIX time

	try:

		while True:
			scanner = BeaconScanner(callback,device_filter=IBeaconFilter(uuid="00050001-0000-1000-8000-00805f9b0131"))
			scanner.start()
			time.sleep(1)
			scanner.stop()
	except Exception as e:
		print(e)
