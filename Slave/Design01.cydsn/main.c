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

#define addr 0x29u // I2C address of VL6180 shifted by 1 bit
                    //(0x29 << 1) so the R/W command can be added
#define MAJOR_OFFSET_Temp_Humi (25u)
#define MAJOR_OFFSET_Press (26u)
#define MINOR_OFFSET_IR (27u)//湿度が格納されている領域
#define MINOR_OFFSET_Capsense (28u)//温度が格納されている領域

#define MINOR_MIN (69u)//０度相当の値
#define MINOR_MAX (126u)//４０度相当の値
#define MINOR_STEP (1u)//minor値の変化量

// Major/Minor フィールドの値
uint8   f_minor = MINOR_MIN;        // Minorフィールドの値,各値を示す,ただしこのままだと１つのtempしか送れない
uint8   f_major = 0;                // Majorフィールドの値,グラフ部分の凡例を示す

// BLE の初期化終了フラグ
uint8   initialized = 0;

// ソフトウェアタイマのカウンタ
uint8   tick = 0;

//最終的なrangeの値が格納される,グローバル変数で宣言された値をcallback関数で使うことで初期化中にread_rangeされるのを防ぐ
int range_value;
int TempC = 0,Humidity = 0;
float Pressure=0;
int temp_humi=0;

//最終的なcapsenseの値が格納される
uint16 capsense_value;

void WriteByte(uint16 reg,uint8 data) {
    
    uint8 data_write[3];
    uint8 SlaveAddress = addr;
    data_write[0] = (reg >> 8) & 0xFF; // MSB of register address 
    data_write[1] = reg & 0xFF; // LSB of register address 
    data_write[2] = data & 0xFF;
    char out[10];
    //using for STM32 F401
    /*
    i2c.write(addr, data_write, 3); 
    */
    uint8 temp;
    /*
    temp = I2C_MasterWriteBuf(SlaveAddress,data_write,3,I2C_MODE_COMPLETE_XFER);//I2C_MasterWriteBuf(SlaveAddress,DataAddress,Byte_Count,I2C_Mode)
    */
     
     SCB_1_I2CMasterClearStatus();
     temp = SCB_1_I2CMasterWriteBuf(addr,data_write,3,SCB_1_I2C_MODE_COMPLETE_XFER);//I2C_MasterWriteBuf(SlaveAddress,DataAddress,Byte_Count,I2C_Mode)
    //temp=I2C_MasterWriteBuf(SlaveAddress,(uint8 *)&reg,1,I2C_MODE_COMPLETE_XFER);//0x02が帰ってきてる
   /* while(1){   
        while(USBUART_1_CDCIsReady()==0u){}
        sprintf(out,"%x",temp);
        USBUART_1_PutString(out);
        while(USBUART_1_CDCIsReady()==0u){}
        USBUART_1_PutCRLF();
    }
    */
   
       
    
    while (temp != SCB_1_I2C_MSTR_NO_ERROR);
    while(SCB_1_I2CMasterStatus() & SCB_1_I2C_MSTAT_XFER_INP);
    temp = SCB_1_I2CMasterClearStatus();
    //return (temp);
    /*while(1){
        sprintf(out,"%x",temp);
        USBUART_1_PutString(out);
        while(USBUART_1_CDCIsReady()==0u){};
        USBUART_1_PutCRLF();
    }*/
    
}

/////////////////////////////////////////////////////////////////// 
// Split 16-bit register address into two bytes and write
// required register address to VL6180 and read the data back 
/////////////////////////////////////////////////////////////////// 

uint8 ReadByte(uint8_t reg) {
    uint8 data_write[2]; 
    uint8 data_read[1];
    uint8 SlaveAddress = addr;
    uint8_t result;
    char out[10];
        uint8 temp;
    
//while(1){     
    data_write[0] = (reg >> 8) & 0xFF; // MSB of register address 
    data_write[1] = reg & 0xFF; // LSB of register address
    //using for STM32 F401
    /*
    i2c.write(addr, data_write, 2); 
    i2c.read(addr, data_read, 1); 
    return data_read[0];
    */
// while(1){       
    SCB_1_I2CMasterClearStatus();
    temp = SCB_1_I2CMasterWriteBuf(addr,data_write,2,SCB_1_I2C_MODE_COMPLETE_XFER);//I2C_MasterWriteBuf(SlaveAddress,DataAddress,Byte_Count,I2C_Mode)
    //temp=I2C_MasterWriteBuf(SlaveAddress,(uint8 *)&reg,1,I2C_MODE_COMPLETE_XFER);//0x02が帰ってきてる
   /* while(1){   
        while(USBUART_1_CDCIsReady()==0u){}
        sprintf(out,"%x",temp);
        USBUART_1_PutString(out);
        while(USBUART_1_CDCIsReady()==0u){}
        USBUART_1_PutCRLF();
    }
    */
    while (temp != SCB_1_I2C_MSTR_NO_ERROR);
    while(SCB_1_I2CMasterStatus() & SCB_1_I2C_MSTAT_XFER_INP);
    temp = SCB_1_I2CMasterClearStatus();
    //return (temp);

    //temp = I2C_MasterReadBuf(addr,data_read,1,I2C_MODE_COMPLETE_XFER);
    temp = SCB_1_I2CMasterReadBuf(SlaveAddress,data_read,1,SCB_1_I2C_MODE_COMPLETE_XFER);
    while (temp != SCB_1_I2C_MSTR_NO_ERROR);
    while(SCB_1_I2CMasterStatus() & SCB_1_I2C_MSTAT_XFER_INP);
    temp = SCB_1_I2CMasterClearStatus();
    return data_read[0];
    /*
    I2C_MasterWriteByte(reg);
    uint8 data ;
    data=I2C_MasterReadByte(0) ;  // ignore error 
    return( data )
    */
     
}

/////////////////////////////////////////////////////////////////// 
// load settings 
/////////////////////////////////////////////////////////////////// 

int VL6180_Init() {
uint8 reset;
uint8 reset_16;
char out[10];
reset = ReadByte(0x016);
 /*
 while(1){
        //reset = ReadByte(0x016);
        
        while(USBUART_1_CDCIsReady()==0u){}
        sprintf(out,"%x",reset);
        USBUART_1_PutString(out);
        while(USBUART_1_CDCIsReady()==0u){}
        USBUART_1_PutCRLF();
        }
*/
 
if (reset==0x01){  // check to see has it be Initialised already
       
    
/////////////////////////////////////////////////////////////////// 
// Added latest settings here - see Section 9
/////////////////////////////////////////////////////////////////// 
     /*while(1){
        //reset = ReadByte(0x016);
        
        while(USBUART_1_CDCIsReady()==0u){}
        USBUART_1_PutString("clear");
        while(USBUART_1_CDCIsReady()==0u){}
        USBUART_1_PutCRLF();
        }
    */

        WriteByte(0x207, 0x01);
        WriteByte(0x208, 0x01);
        WriteByte(0x096, 0x00);
        WriteByte(0x097, 0xFD); // RANGE_SCALER = 253
        WriteByte(0x0E3, 0x00);
        WriteByte(0x0E4, 0x04);
        WriteByte(0x0E5, 0x02);
        WriteByte(0x0E6, 0x01);
        WriteByte(0x0E7, 0x03);
        WriteByte(0x0F5, 0x02);
        WriteByte(0x0D9, 0x05);
        WriteByte(0x0DB, 0xCE);
        WriteByte(0x0DC, 0x03);
        WriteByte(0x0DD, 0xF8);
        WriteByte(0x09F, 0x00);
        WriteByte(0x0A3, 0x3C);
        WriteByte(0x0B7, 0x00);
        WriteByte(0x0BB, 0x3C);
        WriteByte(0x0B2, 0x09);
        WriteByte(0x0CA, 0x09);
        WriteByte(0x198, 0x01);
        WriteByte(0x1B0, 0x17);
        WriteByte(0x1AD, 0x00);
        WriteByte(0x0FF, 0x05);
        WriteByte(0x100, 0x05);
        WriteByte(0x199, 0x05);
        WriteByte(0x1A6, 0x1B);
        WriteByte(0x1AC, 0x3E);
        WriteByte(0x1A7, 0x1F);
        WriteByte(0x030, 0x00);
        
        WriteByte(0x0011, 0x10); // Enables polling for ‘New Sample ready’
                                 // when measurement completes
        WriteByte(0x010a, 0x30); // Set the averaging sample period
                                 // (compromise between lower noise and
                                 // increased execution time)

        WriteByte(0x003f, 0x46); // Sets the light and dark gain (upper
                                 // nibble). Dark gain should not be
                                 // changed.
        WriteByte(0x0031, 0xFF); // sets the # of range measurements after
                                 // which auto calibration of system is
                                 // performed

        WriteByte(0x0041, 0x63); // Set ALS integration time to 100ms
        WriteByte(0x02E,0x01);
        WriteByte(0x01B,0x09);
        WriteByte(0x03E,0x31);
        WriteByte(0x014,0x24);
        WriteByte(0x01C,0x31);
        WriteByte(0x2A3,0);
        
        WriteByte(0x016, 0); //change fresh out of set status to 0
        
    }
    return 0; 
}


/////////////////////////////////////////////////////////////////// 
// Start a range measurement in single shot mode 
/////////////////////////////////////////////////////////////////// 

int VL6180_Start_Range() {
    WriteByte(0x018,0x01);
    return 0; 
}
/////////////////////////////////////////////////////////////////// 
// poll for new sample ready ready 
/////////////////////////////////////////////////////////////////// 

int VL6180_Poll_Range() {
    char status;
    char range_status;
    char out[10];
    // check the status
    status = ReadByte(0x04f); 
    range_status = status & 0x07;
    // wait for new measurement ready status 

    while (range_status != 0x04) {//ここでループしている
        
        //reset = ReadByte(0x016);
       /*
        while(USBUART_1_CDCIsReady()==0u){}
        sprintf(out,"%x",range_status);
        USBUART_1_PutString(out);
        while(USBUART_1_CDCIsReady()==0u){}
        USBUART_1_PutCRLF();
        */
        status = ReadByte(0x04f); 
        range_status = status & 0x07; 
        CyDelay(1); // (can be removed) 
        

        }
      
    return 0; 

}

/////////////////////////////////////////////////////////////////// 
// Read range result (mm) 
/////////////////////////////////////////////////////////////////// 

int VL6180_Read_Range() {
    int range; 
    range=ReadByte(0x062); 
    return range;
}


/////////////////////////////////////////////////////////////////// 
// clear interrupts 
/////////////////////////////////////////////////////////////////// 

int VL6180_Clear_Interrupts() {
    WriteByte(0x015,0x07);
    return 0; 
}


// 500msごとに Advertisement パケットを開始・停止する
void Wdt_Callback(void) {
    int Pressure_int;
    
    if (initialized) {
        // 初期化がされた
        if (tick == 0) {
            // 最初の500msで Advertisement パケットを送信する
           
        
            //y[i] = CapSense_1_sensorRaw[i];
            capsense_value = CapSense_1_ReadSensorRaw(0);//main関数内で実行するとうまく動かない
            
             
            // Major フィールドの設定
            Pressure_int=(Pressure/100)-900;
            cyBle_discoveryData.advData[MAJOR_OFFSET_Press] = Pressure_int;
            //TempC の値を調整
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
            temp_humi=((TempC-10)<<3)|((Humidity-20)/2);
            cyBle_discoveryData.advData[MAJOR_OFFSET_Temp_Humi]=temp_humi;
            
            // Minor フィールドの設定
            
            cyBle_discoveryData.advData[MINOR_OFFSET_Capsense] = capsense_value;
            
            cyBle_discoveryData.advData[MINOR_OFFSET_IR] = range_value;
            
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


// メインループ
int main() {
    CYBLE_API_RESULT_T apiResult;       // エラーコード

    // 割り込みを許可する
    CyGlobalIntEnable;
    
    // 低速タイマ Timer0 のコールバック関数を設定する
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER0, Wdt_Callback);

    // BLE デバイスの初期化を行う
    apiResult = CyBle_Start(StackEventHandler);
    // 初期化が正常に終わったかを確認する
    CYASSERT(apiResult == CYBLE_ERROR_OK);
    CapSense_1_Start();//CapScanON！
    CapSense_1_InitializeAllBaselines();//静電容量の初期化
    CapSense_1_ScanEnabledWidgets();//一度すべてスキャン
    while(CapSense_1_IsBusy() != 0);//センサーのスキャンが完了するまで待つ
   
    SCB_1_Start();
    BME280_start();
    CyDelay(10u);
    VL6180_Init();
    BME280_init();
    for(;;){
       
        CapSense_1_UpdateEnabledBaselines();//センサーの最低値を更新
        CapSense_1_ScanEnabledWidgets();//センサーをスキャンする
        while(CapSense_1_IsBusy() != 0);//センサーのスキャンが完了するまで待つ
        
        
        BME280_init();
        VL6180_Init();
        VL6180_Start_Range();
        VL6180_Poll_Range();
        range_value = VL6180_Read_Range();
        
        
        VL6180_Clear_Interrupts();
        TempC = BME280_readTempC();//温度を入れる
        Pressure = BME280_readFloatPressure();//気圧を入れる
        Humidity = BME280_readFloatHumidity();//湿度を入れる
        // BLE スタックへのイベントを処理する
        CyBle_ProcessEvents();
        
    }
    
}
/* [] END OF FILE */
