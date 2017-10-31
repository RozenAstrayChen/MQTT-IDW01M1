#include "mbed.h"

#ifndef BME280_HPP
#define BME280_HPP

#define BME280_size 14

Serial sensor(PA_11,PA_12);
Serial pc(USBTX,USBRX);
int AT[3] = {0xA5,0x52,0xF7};
class BME280{
public:
    void judgement_data(char buf[]);
    int Temp_read();

private:
    void receive();
    void send_AT();
    char buf[BME280_size];
    int Lux,Temp,Preesure,Hum,Height;

};
void BME280::judgement_data(char buf[]){
    int i=0;
    if(buf[i++]== 0x5A){
        //pc.printf("first_step\n");
        if(buf[i++]== 0x5A){
            //pc.printf("second_step\n");
           //i++;//go to data type
            if(buf[i] == 0x15){
                //pc.printf("Lux type\n");
                i++;//04
                Lux = ((buf[i+1]<<24) | (buf[i+2]<<16) | (buf[i+3]<<8) | buf[i+4])/100;
                //Lux = (0x00<<24) | (0x00<<16) | (0xFE<<8) | 0x40;//test
                //pc.printf("Lux = %d\n", Lux);
            }else if(buf[i] == 0x45){
                //pc.printf("temp type\n");
                i++;//0A
                Temp = ((buf[i+1]<<8)|(buf[i+2]))/100;
                Preesure = ((buf[i+3]<<24)|(buf[i+4]<<16)|(buf[i+5]<<8)|(buf[i+6]))/100;
                Hum = ((buf[i+7]<<8)|(buf[i+8]))/100;
                Height = ((buf[i+9]<<8)|(buf[i+10]));
                //pc.printf("Temp = %d\n", Temp);
                //pc.printf("Preesure = %d\n", Preesure);
                //pc.printf("Hum = %d\n", Hum);
                //pc.printf("Height = %d\n", Height);
            }
        }
    }
}
void BME280::receive(){
    for(int i=0;i<BME280_size;i++){
        buf[i] = sensor.getc();  
        //pc.putc(buf[i]);
    }
    
}
void BME280::send_AT(){
    for(int i=0;i<3;i++){
        sensor.putc(AT[i]);
        //pc.putc(AT[i]);
    }
}
int BME280::Temp_read(){
    send_AT();
    //wait(0.2);
    receive();
    //wait(0.2);
    judgement_data(this->buf);
    
    return Temp;
}

#endif
