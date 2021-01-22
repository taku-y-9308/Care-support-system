/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <stdio.h>
#include "BME280.h"
#include "VL6180x.h"

#define MAJOR_OFFSET_Temp_Humi (25u)
#define MAJOR_OFFSET_Press (26u)
#define MINOR_OFFSET_IR (27u)
#define MINOR_OFFSET_Capsense (28u)

// BLE の初期化終了フラグ
uint8   initialized = 0;

// ソフトウェアタイマのカウンタ
uint8   tick = 0;

/*グローバル変数で宣言された値をcallback関数で使うことでInitializeの前にread_rangeされるのを防ぐ*/

//最終的なrangeの値
int range_value=0;
int TempC = 0,Humidity = 0;
float Pressure=0;
int temp_humi=0;
//最終的なcapsenseの値が格納される
uint16 capsense_value;

// 500msごとに Advertisement パケットを開始・停止する
void Wdt_Callback(void) {
    int Pressure_int;
    
    if (initialized) {
        // 初期化がされた
        if (tick == 0) {
            // 最初の500msで Advertisement パケットを送信する
           
            capsense_value = CapSense_1_ReadSensorRaw(0);//main関数内で実行するとうまく動かない
            
            /* Major フィールドの設定--------------------------------------------*/
            Pressure_int=(Pressure/100)-900;
            cyBle_discoveryData.advData[MAJOR_OFFSET_Press] = Pressure_int;
            /*TempとHumiの値が範囲内に収まるように調整----*/
            //Tempの値を調整
            if(TempC<10){
                TempC=10;   
            }else if(TempC>42){
                TempC=42;   
            }
            //Humiの値を調整
            if(Humidity<20){
                Humidity=20;   
            }else if(Humidity>84){
                Humidity=84;  
            }
            /*---------------------------------------*/
            temp_humi=((TempC-10)<<3)|((Humidity-20)/2);//２つの値を合わせて8bitにする
            cyBle_discoveryData.advData[MAJOR_OFFSET_Temp_Humi]=temp_humi;

            /* ---------------------------------------------------------------*/
            

            /* Minor フィールドの設定--------------------------------------------*/
            
            cyBle_discoveryData.advData[MINOR_OFFSET_Capsense] = capsense_value;
            
            cyBle_discoveryData.advData[MINOR_OFFSET_IR] = range_value;

            /* ---------------------------------------------------------------*/
            
            // Advertisement パケットの送信を開始する
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            tick = 1;
            range_value=0;
            capsense_value=0;
        } else {
            // 次の500msで Advertisement ポケットの送信を止める
            CyBle_GappStopAdvertisement();
            tick = 0;
        }
    }
}



// BLE スタックの処理ハンドラ
void StackEventHandler(uint32 event, void *eventParam) {
    switch (event) {
        // スタックが立ちあがったまたはデバイスが切断されたら初期化終了とみなす
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            initialized = 1;
            break;
        // それ以外のイベントは無視する
        default:
            break;
    }
}

int main() {
    CYBLE_API_RESULT_T apiResult;// エラーコード

    // 割り込みを許可する
    CyGlobalIntEnable;
    
    // 低速タイマ Timer0 のコールバック関数を設定する
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER0, Wdt_Callback);

    // BLE デバイスの初期化を行う
    apiResult = CyBle_Start(StackEventHandler);

    // 初期化が正常に終わったかを確認する
    CYASSERT(apiResult == CYBLE_ERROR_OK);

    CapSense_1_Start();//CapScanON
    CapSense_1_InitializeAllBaselines();//静電容量の初期化
    CapSense_1_ScanEnabledWidgets();//一度すべてスキャン
    while(CapSense_1_IsBusy() != 0);//センサーのスキャンが完了するまで待つ
   
    /*I2CをStart,VL6180xとBME280を初期化--------------------------------------*/
    SCB_1_Start();
    VL6180_Init();
    BME280_init();

    /*----------------------------------------------------------------------*/
    
    for(;;){
       
        CapSense_1_UpdateEnabledBaselines();//センサーの最低値を更新
        CapSense_1_ScanEnabledWidgets();//センサーをスキャンする
        while(CapSense_1_IsBusy() != 0);//センサーのスキャンが完了するまで待つ
        
        BME280_init();
        VL6180_Init();
        VL6180_Start_Range();
        VL6180_Poll_Range();

        /*VL6180xからの値を読み込む----------------------------------------------*/
        range_value = VL6180_Read_Range();

        /*--------------------------------------------------------------------*/
        
        VL6180_Clear_Interrupts();

        /*BME280からの値を読み込む----------------------------------------------*/
        TempC = BME280_readTempC();//温度を入れる
        Pressure = BME280_readFloatPressure();//気圧を入れる
        Humidity = BME280_readFloatHumidity();//湿度を入れる
         /*------------------------------------------------------------------*/

        // BLE スタックへのイベントを処理する
        CyBle_ProcessEvents();
        
    }
    
}
/* [] END OF FILE */
