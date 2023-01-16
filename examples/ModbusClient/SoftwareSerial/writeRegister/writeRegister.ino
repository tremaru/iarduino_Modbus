// ПРИМЕР ЗАПИСИ В ОДИН РЕГИСТР:                                                   //
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
//   Записываем значение для модуля id=9 в нулевой регистр "Coil" (DO):            //
     Serial.print("Coil[0]=1 ");                                                   //
     if( modbus.coilWrite(9, 0, 1) ){ Serial.println("Ok" ); }                     //   Функция возвращает результат записи: true при успехе, false при неудаче.
     else                           { Serial.println("Err"); }                     //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
                                                                                   //
     Serial.print("Coil[0]=0 ");                                                   //
     if( modbus.coilWrite(9, 0, 0) ){ Serial.println("Ok" ); }                     //   Функция возвращает результат записи: true при успехе, false при неудаче.
     else                           { Serial.println("Err"); }                     //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
                                                                                   //
//   Записываем значение для модуля id=9 в нулевой регистр "Holding Register" (AO)://
     Serial.print("HoldingRegister[0]=55555 ");                                    //
     if( modbus.holdingRegisterWrite(9, 0, 55555) ){ Serial.println("Ok" ); }      //   Функция возвращает результат записи: true при успехе, false при неудаче.
     else                                          { Serial.println("Err"); }      //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
                                                                                   //
     Serial.print("HoldingRegister[0]=10000 ");                                    //
     if( modbus.holdingRegisterWrite(9, 0, 10000) ){ Serial.println("Ok" ); }      //   Функция возвращает результат записи: true при успехе, false при неудаче.
     else                                          { Serial.println("Err"); }      //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //