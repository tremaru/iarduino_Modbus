// ПРИМЕР ПОЛУЧЕНИЯ ИНФОРМАЦИИ О ПОСЛЕДНЕЙ ОШИБКЕ:                                 //
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
//   Выполняем любую команду с некорректными данными:                              //
     int8_t regDO = modbus.coilRead(9, 0xF000);                                    //   Читаем из модуля с адресом 9 регистр "Coil" (DO) с адресом 0xF000.
                                                                                   //
//   Выводим сообщение о последней ошибке:                                         //
     if( regDO<0 ){                                                                //
         switch( modbus.lastError() ){                                             //
             case ERROR_ILLEGAL_FUNCTION:    Serial.println( F("Модуль не поддерживает команду или функцию запроса")           ); break;
             case ERROR_ILLEGAL_ADDRESS:     Serial.println( F("Запрос содержит недопустимый адрес регистра")                  ); break;
             case ERROR_ILLEGAL_VALUE:       Serial.println( F("Запрос содержит недопустимое значение в поле данных")          ); break;
             case ERROR_DEVICE_FAILURE:      Serial.println( F("Ошибка запроса не связана с командой, адресом или данными")    ); break;
             case ERROR_ACKNOWLEDGE:         Serial.println( F("Модуль сообщает что занят и обработает запрос позже")          ); break;
             case ERROR_DEVICE_BUSY:         Serial.println( F("Модуль сообщает что занят и не будет обрабатывать запрос")     ); break;
             case ERROR_MEMORY_PARITY:       Serial.println( F("Модуль сообщает что прозошла ошибка чтения/записи файла")      ); break;
             case ERROR_GATEWAY_UNAVAILABLE: Serial.println( F("Шлюз неправильно настроен или перегружен запросами")           ); break;
             case ERROR_GATEWAY_NO_DEVICE:   Serial.println( F("Модуль которому адресован запрос не отвечает")                 ); break;
             case ERROR_SYNTAX:              Serial.println( F("Ошибка синтаксиса")                                            ); break;
             case ERROR_ADR_IARDUINO:        Serial.println( F("Ошибка назначения или сортировки адресов устройств iarduino")  ); break;
             case ERROR_ADR_RESPONSE:        Serial.println( F("Ответ от модуля содержит недопустимый адрес регистра")         ); break;
             case ERROR_VAL_RESPONSE:        Serial.println( F("Ответ от модуля содержит недопустимое значение в поле данных") ); break;
             case ERROR_CRC_RESPONSE:        Serial.println( F("Несовпадение CRC в ответе модуля")                             ); break;
             case ERROR_LEN_REQUEST:         Serial.println( F("Размер запроса к модулю превышает буфер обмена или ADU")       ); break;
             case ERROR_LEN_RESPONSE:        Serial.println( F("Размер ответа от модуля превышает буфер обмена")               ); break;
             default:                        Serial.println( F("Неизвестная ошибка")                                           ); break;
         }                                                                         //
     }else{ Serial.println( F("Ошибки не обнаружены") ); }                         //
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //