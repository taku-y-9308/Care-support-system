/*VL6180x.h*/
#ifndef VL6180X_H
#define VL6180X_H

#define addr 0x29u // I2C address of VL6180x

//センサの各レジスタを初期化する
int VL6180_Init();

int VL6180_Start_Range();

//rangeを測定できる準備ができるまで待つ
int VL6180_Poll_Range();
int VL6180_Read_Range();

int VL6180_Clear_Interrupts();

//****************************************************************************//
//BME280は書き込みにより複雑なコードが必要かつ、ライブラリが存在しているため、そちらを使う
//***************************************************************************//

/*VL6180xに書き込む*/
void VL6180x_WriteByte(uint16 reg,uint8 data)；

/*VL6180xに読み込む*/
uint8 VL6180x_ReadByte(uint8_t reg)；


#endif  // End of VL6180X_H definition check