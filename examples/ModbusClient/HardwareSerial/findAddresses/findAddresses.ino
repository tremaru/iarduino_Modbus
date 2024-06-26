// ПРИМЕР ПОЛУЧЕНИЯ СПИСКА АДРЕСОВ ВСЕХ УСТРОЙСТВ НА ШИНЕ:                         //
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
     Serial.println("Поиск устройств...");                                         //
//   Выполняем поиск всех устройств на шине (занимает несколько секунд):           //
     modbus.findID();                                                              //   Ищем адреса всех устройств на шине. Если указать идентификатор, то будут найдены адреса устройств iarduino указанной модели.
     if( modbus.available() ){                                                     //   Вместо двух строк, можно так: if( modbus.findID() ){
     //  Устройства найдены:                                                       //
         Serial.print("Найдено ");                                                 //
         Serial.print( modbus.available() );                                       //
         Serial.print(" устройств с адресами: ");                                  //
     //  Выводим адреса устройств:                                                 //
         while( modbus.available() ){                                              //
             Serial.print( modbus.read() );                                        //
             Serial.print(", ");                                                   //
         }   Serial.println();                                                     //
//   Если modbus.available() изначально вернуло 0, значит устройства не найдены:   //
     }else{  Serial.println("Не найдено ни одного устройства."); }                 //
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //