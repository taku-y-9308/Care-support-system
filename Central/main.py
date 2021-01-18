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
	global start,weather_info
	IR=(int(additional_info['minor'])>>8)&0xFF
	Capsense=(additional_info['minor'])&0xFF
	temp_humi=(int(additional_info['major'])>>8)&0xFF
	temp=((temp_humi>>3)&0xFF)+10
	humi=(((temp_humi)&0x07)*2)+20
	press=((additional_info['major'])&0xFF)+900
	#POST data
	data={}
	#value from sensors
	data['IRsensor']=IR
	data['Capsense']=Capsense
	data['temp']=temp
	data['humi']=humi
	data['pressure']=press
	data['RSSI']=rssi
	data['weather_info']=weather_info
	#Get weather information every 30 minutes
	if time.time()-start>1800:
		weather_info=get_weather_info.weather_info()
		#Reset the 1800s count by substituing the current UNIX time
		start=time.time()

	dt_now=datetime.datetime.utcnow()
	print(dt_now,data)
	write_to_influxdb(data)

def write_to_influxdb(data):
    json_body = [{
        'measurement': 'result6',
        'time': datetime.datetime.utcnow(),
        'fields': data
    }]
    db.write_points(json_body)

if __name__ == "__main__":

	#Get the current UNIX time
	start=time.time()

	weather_info=get_weather_info.weather_info()
	try:

		while True:
			scanner = BeaconScanner(callback,device_filter=IBeaconFilter(uuid="00050001-0000-1000-8000-00805f9b0131"))
			scanner.start()
			time.sleep(1)
			scanner.stop()
	except Exception as e:
		print(e)
