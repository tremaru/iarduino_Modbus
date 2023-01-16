// ПРИМЕР ЗАПИСИ В НЕСКОЛЬКО РЕГИСТРОВ:                                            //
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
//   Записываем значения для модуля id=9 в регистры "Coils" (DO):                  //
     Serial.print("Coil[0:2]={0,1,0} ");                                           //
     modbus.beginTransmission(9, COILS, 0, 3);                                     //   Инициируем запись для модуля id=9, в регистры "Coils" (DO), начиная с адреса 0, всего 3 значения (3 регистра).
     modbus.write( 0 );                                                            //   Ставим значение в очередь на запись. Это значение будет записано в регистр, адрес которого был указан в modbus.beginTransmission().
     modbus.write( 1 );                                                            //   Ставим значение в очередь на запись. Это значение будет записано в следующий регистр по порядку.
     modbus.write( 0 );                                                            //   Ставим значение в очередь на запись. Это значение будет записано в следующий регистр по порядку.
     if( modbus.endTransmission() ){ Serial.println("Ok" ); }                      //   Выполняем запись. Функция возвращает результат записи: true при успехе, false при неудаче.
     else                          { Serial.println("Err"); }                      //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
                                                                                   //
//   Записываем значения для модуля id=9 в регистры "Holding Registers" (AO):      //
     Serial.print("HoldingRegister[5:8]={11111,22222,33333,44444} ");              //
     modbus.beginTransmission(9, HOLDING_REGISTERS, 5, 4);                         //   Инициируем запись для модуля id=9, в регистры "Holding Registers" (AO), начиная с адреса 5, всего 4 значения (4 регистра).
     modbus.write( 11111 );                                                        //   Ставим значение в очередь на запись. Это значение будет записано в регистр, адрес которого был указан в modbus.beginTransmission().
     modbus.write( 22222 );                                                        //   Ставим значение в очередь на запись. Это значение будет записано в следующий регистр по порядку.
     modbus.write( 33333 );                                                        //   Ставим значение в очередь на запись. Это значение будет записано в следующий регистр по порядку.
     modbus.write( 44444 );                                                        //   Ставим значение в очередь на запись. Это значение будет записано в следующий регистр по порядку.
     if( modbus.endTransmission() ){ Serial.println("Ok" ); }                      //   Выполняем запись. Функция возвращает результат записи: true при успехе, false при неудаче.
     else                          { Serial.println("Err"); }                      //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
}                                                                                  //
                                                                                   //
//   При записи в несуществующие регистры, modbus.endTransmission() вернёт 0.      //
//   Пример:                                                                       //
//   Предположим, у модуля есть 7 регистров "HoldingRegister" (AO) с адресами 0-6. //
//   Попытка записи в 4 регистра с адресами 5-8, как в примере, приведёт к ошибке. //
//   У модуля есть регистры с адресами 5 и 6, но нет регистров с адресами 7 и 8.   //
//   modbus.endTransmission() вернёт 0 и ни одно значение не будет записано.       //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //