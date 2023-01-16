// ПРИМЕР ЧТЕНИЯ ОДНОГО РЕГИСТРА:                                                  //
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
//   Читаем из модуля с адресом 9 нулевой регистр "Coil" (DO):                     //
     int8_t regDO = modbus.coilRead(9, 0);                                         //   Функция возвращает прочитанное значение (0/1), или -1 при неудаче.
     Serial.print("Coil[0]=");                                                     //
     if( regDO<0 ){ Serial.println("Err"); }else{ Serial.println(regDO); }         //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
                                                                                   //
//   Читаем из модуля с адресом 9 нулевой регистр "Discrete Input" (DI):           //
     int8_t regDI = modbus.discreteInputRead(9, 0);                                //   Функция возвращает прочитанное значение (0/1), или -1 при неудаче.
     Serial.print("DiscreteInput[0]=");                                            //
     if( regDI<0 ){ Serial.println("Err"); }else{ Serial.println(regDI); }         //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
                                                                                   //
//   Читаем из модуля с адресом 9 нулевой регистр "Holding Register" (AO):         //
     int32_t regAO = modbus.holdingRegisterRead(9, 0);                             //   Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
     Serial.print("HoldingRegister[0]=");                                          //
     if( regAO<0 ){ Serial.println("Err"); }else{ Serial.println(regAO); }         //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
                                                                                   //
//   Читаем из модуля с адресом 9 нулевой регистр "Input Register" (AI):           //
     int32_t regAI = modbus.inputRegisterRead(9, 0);                               //   Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
     Serial.print("InputRegister[0]=");                                            //
     if( regAI<0 ){ Serial.println("Err"); }else{ Serial.println(regAI); }         //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //