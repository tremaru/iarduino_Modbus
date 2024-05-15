// ПРИМЕР ПОЛУЧЕНИЯ НАЗВАНИЯ МОДУЛЯ iArduino:                                                     //
                                                                                                  //
#include <iarduino_Modbus.h>                                                                      //   Подключаем библиотеку для работы по протоколу Modbus.
ModbusClient modbus(Serial1, 2);                                                                  //   Создаём объект для работы по протоколу Modbus указывая объект шины UART-RS485 для протокола и вывод DE конвертера UART-RS485.
                                                                                                  //
void setup(){                                                                                     //
     Serial.begin(9600); while(!Serial);                                                          //   Инициируем передачу данных в монитор последовательного порта, указав его скорость.
     Serial1.begin(9600); while(!Serial1);                                                        //   Инициируем работу с шиной UATR-RS485 указав её скорость.
     modbus.begin();                                                                              //   Инициируем работу по протоколу Modbus.
//   modbus.setTimeout(10);                                                                       //   Указываем максимальное время ожидания ответа по протоколу Modbus.
//   modbus.setDelay(4);                                                                          //   Указываем минимальный интервал между отправляемыми сообщениями по протоколу Modbus.
//   modbus.setTypeMB( MODBUS_RTU );                                                              //   Указываем тип протокола Modbus: MODBUS_RTU (по умолчанию), или MODBUS_ASCII.
                                                                                                  //
//   Получаем название модуля с адресом 9:                                                        //
     int8_t lenName = modbus.getName(9);                                                          //   Функция возвращает длину строки с названием устройства, доступную для чтения функцией read() и available().
     if( lenName>0 ){                                                                             //
         Serial.print(F("Название модуля: «"));                                                   //
         while( modbus.available() ){ Serial.print( (char)modbus.read() ); }                      //   Выводим название побайтно, используя функции available() и read().
         Serial.println("».");                                                                    //
     }else{ Serial.println(F("Название модуля неизвестно")); }                                    //
}                                                                                                 //
                                                                                                  //
void loop(){                                                                                      //
}                                                                                                 //