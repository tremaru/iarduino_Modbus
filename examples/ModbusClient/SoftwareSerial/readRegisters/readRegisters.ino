// ПРИМЕР ЧТЕНИЯ НЕСКОЛЬКИХ РЕГИСТРОВ:                                             //
                                                                                   //
#include <SoftwareSerial.h>                                                        //   Подключаем библиотеку для работы с программной шиной UART.
#include <iarduino_Modbus.h>                                                       //   Подключаем библиотеку для работы по протоколу Modbus.
                                                                                   //
SoftwareSerial     rs485(8,9);                                                     //   Создаём объект для работы с программной шиной UART-RS485 указывая выводы RX, TX.
ModbusClient       modbus(rs485, 2);                                               //   Создаём объект для работы по протоколу Modbus указывая объект программной шины UART-RS485 и вывод DE конвертера UART-RS485.
                                                                                   //
void setup(){                                                                      //
     Serial.begin(9600);   while(!Serial);                                         //   Инициируем передачу данных в монитор последовательного порта, указав его скорость.
     rs485.begin(9600); // while(!rs485 );                                         //   Инициируем работу с программной шиной UATR-RS485 указав её скорость.
     modbus.begin();                                                               //   Инициируем работу по протоколу Modbus.
//   modbus.setTimeout(10);                                                        //   Указываем максимальное время ожидания ответа по протоколу Modbus.
//   modbus.setDelay(4);                                                           //   Указываем минимальный интервал между отправляемыми сообщениями по протоколу Modbus.
//   modbus.setTypeMB( MODBUS_RTU );                                               //   Указываем тип протокола Modbus: MODBUS_RTU (по умолчанию), или MODBUS_ASCII.
                                                                                   //
//   Читаем из модуля id=9, регистры "Coils" (DO), с адреса 0, всего 3 регистра:   //   Получается что мы читаем регистры с адресами 0, 1 и 2.
     Serial.print("Coils = {");                                                    //
     if( !modbus.requestFrom(9, COILS, 0, 3) ){ Serial.print("Err"); }             //   Функция modbus.requestFrom() возвращает количество прочитанных значений, или 0 при неудаче.
     else{ while(modbus.available()){ Serial.print((String) " "+modbus.read()); }} //   Читаем полученные значения функцией modbus.read() проверяя их наличие функцией modbus.available().
     Serial.println(" }");                                                         //
                                                                                   //
//   Читаем из модуля id=9, регистры "Discrete Inputs" (DI), с адреса 1, всего 2:  //   Получается что мы читаем регистры с адресами 1 и 2.
     Serial.print("DiscreteInputs = {");                                           //
     if( !modbus.requestFrom(9, DISCRETE_INPUTS, 1, 2) ){ Serial.print("Err"); }   //   Функция modbus.requestFrom() возвращает количество прочитанных значений, или 0 при неудаче.
     else{ while(modbus.available()){ Serial.print((String) " "+modbus.read()); }} //   Читаем полученные значения функцией modbus.read() проверяя их наличие функцией modbus.available().
     Serial.println(" }");                                                         //
                                                                                   //
//   Читаем из модуля id=9, регистры "Holding Registers" (AO), с адреса 0, всего 1://   Получается что мы читаем регистр с адресом 0.
     Serial.print("HoldingRegisters = {");                                         //
     if( !modbus.requestFrom(9, HOLDING_REGISTERS, 0, 1) ){ Serial.print("Err"); } //   Функция modbus.requestFrom() возвращает количество прочитанных значений, или 0 при неудаче.
     else{ while(modbus.available()){ Serial.print((String) " "+modbus.read()); }} //   Читаем полученные значения функцией modbus.read() проверяя их наличие функцией modbus.available().
     Serial.println(" }");                                                         //
                                                                                   //
//   Читаем из модуля id=9, регистры "Input Registers" (AI), с адреса 5, всего 6:  //   Получается что мы читаем регистры с адресами 5, 6, 7, 8, 9 и 10.
     Serial.print("InputRegisters = {");                                           //
     if( !modbus.requestFrom(9, INPUT_REGISTERS, 5, 6) ){ Serial.print("Err"); }   //   Функция modbus.requestFrom() возвращает количество прочитанных значений, или 0 при неудаче.
     else{ while(modbus.available()){ Serial.print((String) " "+modbus.read()); }} //   Читаем полученные значения функцией modbus.read() проверяя их наличие функцией modbus.available().
     Serial.println(" }");                                                         //
}                                                                                  //
                                                                                   //
//   При попытке чтения несуществующих регистров, modbus.requestFrom() вернёт 0.   //
//   Пример:                                                                       //
//   Предположим, у модуля есть 7 регистров "Input Registers" (AI) с адресами 0-6. //
//   Попытка чтения 6 регистров с адресами 5-10, как в примере, приведёт к ошибке. //
//   У модуля есть регистры с адресами 5 и 6, но нет регистров с адресами 7-10.    //
//   modbus.requestFrom() вернёт 0 и ни один регистр не будет прочитан.            //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //