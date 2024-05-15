// ПРИМЕР ЗАПИСИ В ОДИН РЕГИСТР "HOLDING REGISTER" (AO) С МАСКАМИ AND И OR:        //
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
//   Записываем значение для модуля id=9 в нулевой регистр "Holding Register" (AO)://
     Serial.print("HoldingRegister[0]=0xABXX ");                                   //
     bool i = modbus.registerMaskWrite(9, 0, 0b0000000011111111, 0xABCD );         //   Для модуля id=9, в нулевой регистр "Holding Register" (AO), записываем маску AND (0b0000000011111111) и OR (0xABCD).
     if( i ){ Serial.println("Ok"); }else{ Serial.println("Err"); }                //   Тип (код) последней ошибки можно получить функцией modbus.lastError().
}                                                                                  //
                                                                                   //
//   Маска AND содержит 16 флагов установка которых запрещает менять биты регистра.//
//   Маска OR содержит 16 бит данных для записи (если это не запрещено маской AND).//
//   Пример:                                                                       //
//   Маска AND (0b0000000011111111) запрещает менять последние 8 бит регистра.     //
//   Маска OR (0xABCD) содержит данные для записи.                                 //
//   Значит старшие 8 бит регистра "Holding Register" (AO) изменятся на значение   //
//   из маски OR, а младшие 8 бит регистра останутся без изменений.                //
//   Если HoldingRegister[0] был равен 0x1234, то после записи масок из примера,   //
//   его занчение изменится на 0xAB34.                                             //
//   Это позволяет одной командой изменить только часть данных регистра.           //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //