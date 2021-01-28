/*VL6180x.h*/
#ifndef VL6180X_H
#define VL6180X_H

#include <project.h>
#define addr 0x29u // I2C address of VL6180x
#define Q1 41.6//Pre-calibration
#define Q2 22//Ranging
#define Q3 25//Readout averaging

#define RESULT__RANGE_RETURN_CONV_TIME 0x07C
#define READOUT__AVERAGING_SAMPLE_PERIOD 0x10A

//センサの各レジスタを初期化する
int VL6180_Init();

int VL6180_Start_Range();

//rangeを測定できる準備ができるまで待つ
int VL6180_Poll_Range();
int VL6180_Read_Range();

int VL6180_Clear_Interrupts();

//****************************************************************************//
//WriteByteとReadByteに関しては、BME280は書き込みにより複雑なコードが必要かつ、
//ライブラリが存在しているため別々の関数で動かしている
//***************************************************************************//

/*VL6180xに書き込む*/
void VL6180x_WriteByte(uint16, uint8);

/*VL6180xに読み込む*/
uint8 VL6180x_ReadByte(uint8_t);

float VL6180_Current_Consumption();


#endif  // End of VL6180X_H definition check