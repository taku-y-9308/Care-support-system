import requests
import json
import pprint

def weather_info():
    #県大の位置情報　34.87705828809757, 134.6497809192033　を使用
    url = 'https://api.openweathermap.org/data/2.5/onecall?lat=4.87705828809757&lon=134.6497809192033&appid=d446ff284fb2c6a9d53eba7cab4a4fec'

    res = requests.get(url)

    lists = json.loads(res.text)
    weather_info=int(json.dumps(lists['current']['weather'][0]['id'],indent=2))
    #現在の状態だけを表示、mainの中身だけ
    #pprint.pprint(lists['current']['weather'][0]['main'])

    return weather_info
    #全ての値を表示
    #pprint.pprint(lists)
if __name__ == "__main__":
    weather_info()
