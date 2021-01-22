/*VL6180x.c*/
#include "VL6180x.h"
#include <math.h>

//****************************************************************************//
//WriteByteとReadByteに関しては、BME280は書き込みにより複雑なコードが必要かつ、
//ライブラリが存在しているため別々の関数で動かしている
//***************************************************************************//

void VL6180x_WriteByte(uint16 reg,uint8 data) {
    
    uint8 data_write[3];
    uint8 SlaveAddress = addr;
    data_write[0] = (reg >> 8) & 0xFF; // MSB of register address 
    data_write[1] = reg & 0xFF; // LSB of register address 
    data_write[2] = data & 0xFF;
    char out[10];
    
    uint8 temp;
    
    SCB_1_I2CMasterClearStatus();
    temp = SCB_1_I2CMasterWriteBuf(addr,data_write,3,SCB_1_I2C_MODE_COMPLETE_XFER);
    
    while (temp != SCB_1_I2C_MSTR_NO_ERROR);
    while(SCB_1_I2CMasterStatus() & SCB_1_I2C_MSTAT_XFER_INP);
    temp = SCB_1_I2CMasterClearStatus();
    
}

//****************************************************************************//
// 16ビットのレジスタアドレスを2バイトに分割し、
//必要なレジスタアドレスをVL6180に書き込み、データを読み出す。
//***************************************************************************//
uint8 VL6180x_ReadByte(uint8_t reg) {
    uint8 data_write[2]; 
    uint8 data_read[1];
    uint8 SlaveAddress = addr;
    uint8_t result;
    char out[10];
    uint8 temp;
    
    data_write[0] = (reg >> 8) & 0xFF; // MSB of register address 
    data_write[1] = reg & 0xFF; // LSB of register address
    
    SCB_1_I2CMasterClearStatus();
    temp = SCB_1_I2CMasterWriteBuf(addr,data_write,2,SCB_1_I2C_MODE_COMPLETE_XFER);
    
    while (temp != SCB_1_I2C_MSTR_NO_ERROR);
    while(SCB_1_I2CMasterStatus() & SCB_1_I2C_MSTAT_XFER_INP);
    temp = SCB_1_I2CMasterClearStatus();

    temp = SCB_1_I2CMasterReadBuf(SlaveAddress,data_read,1,SCB_1_I2C_MODE_COMPLETE_XFER);
    while (temp != SCB_1_I2C_MSTR_NO_ERROR);
    while(SCB_1_I2CMasterStatus() & SCB_1_I2C_MSTAT_XFER_INP);
    temp = SCB_1_I2CMasterClearStatus();
    return data_read[0];
     
}

//***************************************************************************//
// 各レジスタを初期化
//***************************************************************************//

int VL6180_Init() {
uint8 reset;
uint8 reset_16;
char out[10];
reset = ReadByte(0x016);
 
if (reset==0x01){  // check to see has it be Initialised already
       
        //各レジスタの初期化***********************************//
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
        //***************************************************//

        /*statusを変更*/
        WriteByte(0x016, 0);
        
    }
    return 0; 
}

//***************************************************************************//
// single shot modeでレンジ測定を開始 
//***************************************************************************//

int VL6180_Start_Range() {
    WriteByte(0x018,0x01);
    return 0; 
}

//***************************************************************************//
// 新しいサンプルの準備ができているかどうかの確認
//***************************************************************************//

int VL6180_Poll_Range() {
    char status;
    char range_status;
    char out[10];
    // statusをチェックする
    status = ReadByte(0x04f); 
    range_status = status & 0x07;
    
    // 測定準備が完了するまでを待つ
    while (range_status != 0x04) {
        status = ReadByte(0x04f); 
        range_status = status & 0x07; 
        CyDelay(1); 
    }
      
    return 0; 

}

//***************************************************************************//
//range result(mm)を読み出す 
//***************************************************************************//

int VL6180_Read_Range() {
    int range; 
    range=ReadByte(0x062); 
    return range;
}
//***************************************************************************//
// clear interrupts 
//***************************************************************************//

int VL6180_Clear_Interrupts() {
    WriteByte(0x015,0x07);
    return 0; 
}