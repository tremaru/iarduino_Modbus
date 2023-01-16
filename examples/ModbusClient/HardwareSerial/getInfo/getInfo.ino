// ПРИМЕР ПОЛУЧЕНИЯ ИНФОРМАЦИИ О УСТРОЙСТВЕ:                                                      //
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
//   Проверяем адрес 9 на шине Modbus:                                                            //
     uint8_t adr = modbus.checkID(9);                                                             //   Функция возвращает DEVICE_MB_ABSENT/DEVICE_MB_DOUBLE/DEVICE_MB_UNKNOWN/DEVICE_MB_IARDUINO.
     Serial.print(F("Адрес 9 на шине Modbus принадлежит "));                                      //
     switch(adr){                                                                                 //
         case DEVICE_MB_ABSENT:   Serial.println(F("отсутствующему устройству."  )); break;       //   Нет устройства.
         case DEVICE_MB_DOUBLE:   Serial.println(F("двум и более устройствам."   )); break;       //   Несколько устройств с одинаковым адресом.
         case DEVICE_MB_UNKNOWN:  Serial.println(F("устройству не iarduino."     )); break;       //   Устройство не iarduino.
         case DEVICE_MB_IARDUINO: Serial.println(F("устройству линейки iarduino.")); break;       //   Устройство iarduino.
     }                                                                                            //
                                                                                                  //
//   Получаем состояние 8 статусных выходов модуля с адресом 9:                                   //
     int16_t pins = modbus.exceptionStatusRead(9);                                                //   Функция возвращает байт битов, или -1 при неудаче.
     Serial.print(F("Состояние статусных выходов: "));                                            //
     if( pins<0 ){ Serial.println(F("неопределено")); }                                           //
     else{ Serial.println( (String) "1="+bitRead(pins,0)+", 2="+bitRead(pins,1)+", 3="+bitRead(pins,2)+", 4="+bitRead(pins,3)+", 5="+bitRead(pins,4)+", 6="+bitRead(pins,5)+", 7="+bitRead(pins,6)+", 8="+bitRead(pins,7)+"."); }
                                                                                                  //
//   Получаем текущее состояние модуля с адресом 9:                                               //
     int8_t state = modbus.getSate(9);                                                            //   Функция возвращает бит состояния (0-свободно,1-занято), или -1 при неудаче.
     Serial.print(F("Состояние устройства: "));                                                   //
     if( pins==0 ){ Serial.println(F("свободно."    )); }else                                     //
     if( pins==1 ){ Serial.println(F("занято."      )); }else                                     //
                  { Serial.println(F("неопределено.")); }                                         //
                                                                                                  //
//   Получаем количество успешно выполненых запросов модулем с адресом 9:                         //
     int8_t counter = modbus.getEventCounter(9);                                                  //   Функция возвращает значение счётчика событий, или -1 при неудаче.
     Serial.print(F("Модуль успешно выполнил "));                                                 //
     if( pins<0 ){ Serial.println(F("неизвестное количество запросов")); }                        //
     else{ Serial.println( (String) counter+" запросов."); }                                      //
                                                                                                  //
//   Получаем содержимое журнала событий модуля с адресом 9:                                      //
     int8_t lenLog = modbus.getEventLog(9);                                                       //   Функция возвращает количество данных, доступных для чтения функцией read() и available().
     if( lenLog==0 ){ Serial.println(F("Данные журнала событий неизвестны")); }                   //
     else           { Serial.println(F("Данные журнала событий:")); }                             //
//   Выводим слово состояния и значение двух счётчиков:                                           //
     if( modbus.available() ){ Serial.println((String)"- слово состояния = "  +modbus.read() ); } //   Первое значение возвращаемое функцией read() - слово состояния: FFFF - занят выполнением предыдущей команды, 0000 - сводобен.
     if( modbus.available() ){ Serial.println((String)"- счётчик событий = "  +modbus.read() ); } //   Второе значение возвращаемое функцией read() - счётчик событий: содержит количество успешно выполненных запросов.
     if( modbus.available() ){ Serial.println((String)"- счётчик сообщений = "+modbus.read() ); } //   Третье значение возвращаемое функцией read() - счётчик сообщений: содержит количество всех обнаруженных запросов на шине.
//   Выводим байты журнала событий:                                                               //
     if( modbus.available() ){ Serial.print(F("- байты журнала:")); }                             //
     while( modbus.available() ){ Serial.print(' '); Serial.print(modbus.read()); }               //   Остальные значения возвращаемые функцией read() - байты: приёма запроса, обработки сообщений, выполнения перезагрузки и режима Listen Only Mode.
     Serial.println('.');                                                                         //
                                                                                                  //
//   Получаем информацию о модуле с адресом 9:                                                    //
     int8_t lenInfo = modbus.getInfo(9);                                                          //   Функция возвращает количество байт данных об устройстве, доступных для чтения функцией read() и available().
     if( lenInfo==0 ){ Serial.println(F("Информация о модуле неизвестна")); }                     //
     else            { Serial.println(F("Информация о модуле:")); }                               //
//   Выводим идентификатор линейки устройств и индикатор пуска:                                   //
     if( modbus.available() ){ Serial.print(F("- идентификатор линейки устройств = ")); Serial.println( modbus.read() ); } // Первое значение возвращаемое функцией read() - идентификатор линейки устройств, уникально для производителей.
     if( modbus.available() ){ Serial.print(F("- индикатор пуска = "));                 Serial.println( modbus.read() ); } // Второе значение возвращаемое функцией read() - индикатор пуска: 0x00=OFF, 0xFF=ON.
//   Выводим остальные данные в зависимости от устройства:                                        //
     if( adr!=DEVICE_MB_IARDUINO ){                                                               //   Переменная adr определена в начале скетча при проверке адреса на шине Modbus.
     //  Для устройства не iarduino:                                                              //
         Serial.print(F("- остальные байты информации:"));                                        //
         while( modbus.available() ){ Serial.print(' '); Serial.print(modbus.read()); }           //   Остальные значения возвращаемые функцией read() - байты значений которые зависят от производителя и устройства.
         Serial.println('.');                                                                     //
     }else{                                                                                       //
     //  Для устройства принадлежащего линейке iarduino:                                          //
         if( modbus.available()    ){ Serial.print(F("- адрес на шине = "));            Serial.println( modbus.read() ); } // Третье значение для устройств iarduino возвращаемое функцией read() - адрес на шине.
         if( modbus.available()    ){ Serial.print(F("- идентификатор устройства = ")); Serial.println( modbus.read() ); } // Идентификатор устройства определяет тип (модель) устройства iarduino.
         if( modbus.available()    ){ Serial.print(F("- версия прошивки = "));          Serial.println( modbus.read() ); } // Версия прошивки от 1 до 255.
         if( modbus.available()>=2 ){ Serial.print(F("- регистр диагностики = "));      Serial.println((modbus.read()<<8)|modbus.read() ); } // Регистр диагностики состоит из 2 байт, каждый бит которых информирует об ошибке.
         if( modbus.available()    ){ Serial.print(F("- количество регистров DO = "));  Serial.println( modbus.read() ); } // Регистры DO это регистры Coils.
         if( modbus.available()    ){ Serial.print(F("- количество регистров DI = "));  Serial.println( modbus.read() ); } // Регистры DI это регистры Discrete Inputs.
         if( modbus.available()    ){ Serial.print(F("- количество регистров AO = "));  Serial.println( modbus.read() ); } // Регистры AO это регистры Holding Registers.
         if( modbus.available()    ){ Serial.print(F("- количество регистров AI = "));  Serial.println( modbus.read() ); } // Регистры AI это регистры Input Registers.
         if( modbus.available()>=4 ){ Serial.print(F("- задержка до ответа в мкс = ")); Serial.println((modbus.read()<<24)|(modbus.read()<<16)|(modbus.read()<<8)|modbus.read() ); } // Время задержки занимает 4 байта.
     }                                                                                            //
}                                                                                                 //
                                                                                                  //
void loop(){                                                                                      //
}                                                                                                 //