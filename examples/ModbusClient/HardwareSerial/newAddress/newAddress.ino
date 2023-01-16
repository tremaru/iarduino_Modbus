// ПРИМЕР СМЕНЫ АДРЕСА УСТРОЙСТВА IARDUINO:                                        //
                                                                                   //
uint8_t nowAddress = 9;                                                            //   Текущий адрес ( 1 - 247 ).
uint8_t newAddress = 27;                                                           //   Новый адрес ( 1 - 247 ).
                                                                                   //
#include <iarduino_Modbus.h>                                                       //   Подключаем библиотеку для работы по протоколу Modbus.
ModbusClient modbus(Serial1, 2);                                                   //   Создаём объект для работы по протоколу Modbus указывая объект шины UART-RS485 для протокола и вывод DE конвертера UART-RS485.
                                                                                   //
void setup(){                                                                      //
     Serial.begin(9600); while(!Serial);                                           //   Инициируем передачу данных в монитор последовательного порта, указав его скорость.
     Serial1.begin(9600); while(!Serial1);                                         //   Инициируем работу с шиной UATR-RS485 указав её скорость.
     modbus.begin();                                                               //   Инициируем работу по протоколу Modbus.
//   modbus.setTimeout(10);                                                        //   Указываем максимальное время ожидания ответа по протоколу Modbus.
//   modbus.setDelay(4);                                                           //   Указываем минимальный интервал между отправляемыми сообщениями по протоколу Modbus.
//   modbus.setTypeMB( MODBUS_RTU );                                               //   Указываем тип протокола Modbus: MODBUS_RTU (по умолчанию), или MODBUS_ASCII.
                                                                                   //
//   Меняем адрес устройства:                                                      //
     bool f = modbus.changeID(nowAddress, newAddress);                             //   Меняем адрес устройства iarduino с nowAddress на newAddress.
              Serial.print("Адрес ");                                              //
              Serial.print(nowAddress);                                            //
     if(!f){  Serial.print(" не"); }                                               //
              Serial.print(" изменён на адрес ");                                  //
              Serial.print(newAddress);                                            //
              Serial.println('.');                                                 //
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //