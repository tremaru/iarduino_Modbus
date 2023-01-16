// ПРИМЕР СМЕНЫ АДРЕСА УСТРОЙСТВА IARDUINO:                                        //
                                                                                   //
uint8_t nowAddress = 9;                                                            //   Текущий адрес ( 1 - 247 ).
uint8_t newAddress = 27;                                                           //   Новый адрес ( 1 - 247 ).
                                                                                   //
#include <SoftwareSerial.h>                                                        //   Подключаем библиотеку для работы с программной шиной UART.
#include <iarduino_Modbus.h>                                                       //   Подключаем библиотеку для работы по протоколу Modbus.
                                                                                   //
SoftwareSerial     rs485(8,9);                                                     //   Создаём объект для работы с программной шиной UART-RS485 указывая выводы RX, TX.
ModbusClient       modbus(rs485, 2);                                               //   Создаём объект для работы по протоколу Modbus указывая объект программной шины UART-RS485 и вывод DE конвертера UART-RS485.
                                                                                   //
void setup(){                                                                      //
     Serial.begin(9600); while(!Serial);                                           //   Инициируем передачу данных в монитор последовательного порта, указав его скорость.
     rs485.begin(9600); while(!rs485);                                             //   Инициируем работу с программной шиной UATR-RS485 указав её скорость.
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