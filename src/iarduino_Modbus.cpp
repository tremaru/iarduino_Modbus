#include "iarduino_Modbus.h"																								//
																															//
//		КОНСТРУКТОР КЛАССА:																									//
		ModbusClient::ModbusClient(Stream &obj, uint8_t pin){ objSerial=&obj; pinDE=pin; }									//	&obj - ссылка на объект для работы с аппаратным или программным UART, pin - № вывода Arduino к которому подключён вывод DE конвертера UART - RS485.
																															//
//		ФУНКЦИЯ ИНИЦИАЛИЗАЦИИ РАБОТЫ ПО ПРОТОКОЛУ MODBUS:																	//
void	ModbusClient::begin(void){																							//
			if( pinDE<0xFF ){																								//
				pinMode(pinDE, OUTPUT);																						//	Переводим вывод pinDE в режим выхода.
				digitalWrite(pinDE, LOW);																					//	Устанавливаем на выводе pinDE уровень логического 0 (запрещаем передачу).
			}	delay(300);																									//	Задержка для программной шины UART.
}																															//
																															//
//		ФУНКЦИЯ ПОИСКА УСТРОЙСТВ IARDUINO С ОДИНАКОВЫМИ АДРЕСАМИ:															//	Возвращает количество найденных ID, доступных для чтения функцией read() и available().
uint8_t	ModbusClient::searchERR(void){																						//
			uint8_t j[31]; for(uint8_t i=0; i<31; i++){ j[i]=0; }															//	Определяем массив битовых флагов.
			for(uint8_t i=1; i<=247; i++){ if( checkID(i)==DEVICE_MB_DOUBLE ){ j[i/8]|=(1<<(i%8)); }}						//	Находим адреса с двумя устройствами и добавляем их в битовый список j.
		//	Собираем данные из списка j в массив arrADU для функций read() и available():									//
			requestSUM=0; for(uint8_t i=1; i<=247; i++){ if( ((requestSUM+2)<MODBUS_ADU_MAX) && (j[i/8]&(1<<(i%8))) ){ arrADU[(requestSUM+2)]=i; requestSUM++; }} // Сохраняем ID найденных устройств из битового списка j в массив iarrADU.
			arrADU[1]=0x05; requestCNT=requestSUM;																			//	Определяем идентификатор для функции read() и количество найденных ID устройств доступных для чтения функцией read().
			return requestSUM;																								//	Возвращаем количество найденных ID устройств.
}																															//
																															//
//		ФУНКЦИЯ ПОИСКА ID ВСЕХ УСТРОЙСТВ УКАЗАННОЙ МОДЕЛИ:																	//	Возвращает количество найденных ID, доступных для чтения функцией read() и available().
uint8_t	ModbusClient::findID(uint8_t mod){																					//	mod - номер (идентификатор) искомой модели, или 0 если нужно найти все устройства на шине.
			uint8_t j[31]; for(uint8_t i=0; i<31; i++){ j[i]=0; }															//	Определяем массив битовых флагов.
			for(uint8_t i=1; i<=247; i++){																					//	Проходим по всем адресам.
				if( checkID(i)!=DEVICE_MB_ABSENT ){																			//	Если устройство не отсутствует.
					if(!mod)                                                            { j[i/8]|=(1<<(i%8)); }else			//	Добавляем все найденные устройства    в битовый список j.
					if(getInfo(i)){if(read()==0x77){read();if(read()==i){if(read()==mod){ j[i/8]|=(1<<(i%8)); }}}}			//	Добавляем устройство указанной модели в битовый список j.
				}																											//
			}																												//
		//	Собираем данные из списка j в массив arrADU для функций read() и available():									//
			requestSUM=0; for(uint8_t i=1; i<=247; i++){ if( ((requestSUM+2)<MODBUS_ADU_MAX) && (j[i/8]&(1<<(i%8))) ){ arrADU[(requestSUM+2)]=i; requestSUM++; }} // Сохраняем ID найденных устройств из битового списка j в массив iarrADU.
			arrADU[1]=0x05; requestCNT=requestSUM;																			//	Определяем идентификатор для функции read() и количество найденных ID устройств доступных для чтения функцией read().
			return requestSUM;																								//	Возвращаем количество найденных ID устройств.
}																															//
																															//
//		ФУНКЦИЯ ИЗМЕНЕНИЯ ID УСТРОЙСТВ IARDUINO:																			//	Возвращает количество устройств которым был изменён ID, или -1 при неудаче.
int16_t	ModbusClient::sortID(uint8_t start){																				//	start - первый адрес в новом списке устройств.
			uint8_t arrI[31];																								//	Объявляем массив битовых флагов адресов принадлежащих устройствам линейки iarduino.
			uint8_t arrU[31];																								//	Объявляем массив битовых флагов адресов принадлежащих сторонним устройствам.
			uint8_t cntCHANGE=0;																							//	Определяем счётчик устройств которым был изменён адрес.
			uint8_t cntDOUBLE=20;																							//	Определяем счётчик обнаружения адреса принадлежащего нескольким устройствам.
			bool    flgDOUBLE=false;																						//	Определяем флаг    обнаружения адреса принадлежащего нескольким устройствам.
		//	Сбрасываем широковещательным запросом регистр "Coil" DO[0x0100]:												//
			coilWrite(0x0100, 0);																							//	Регистр устанавливается самостоятельно после подтверждения смены адреса.
		//	Избавляемся от дублированных адресов:																			//
			do{ flgDOUBLE=false;																							//
			//	Проверяем все адреса:																						//
				for(uint8_t i=1, j; i<=247; i++){																			//	Проходим по всем адресам.
					j=checkID(i);																							//	Проверяем адрес i.
					if(j==DEVICE_MB_IARDUINO){ arrI[i/8]|=(1<<(i%8)); }else{ arrI[i/8]&=~(1<<(i%8)); }						//	Устанавливаем флаг обнаружения адреса принадлежащего устройству линейки iarduino.
					if(j==DEVICE_MB_UNKNOWN ){ arrU[i/8]|=(1<<(i%8)); }else{ arrU[i/8]&=~(1<<(i%8)); }						//	Устанавливаем флаг обнаружения адреса принадлежащего стороннему устройству.
					if(j==DEVICE_MB_DOUBLE  ){ flgDOUBLE=true; }															//	Устанавливаем флаг обнаружения адреса принадлежащего нескольким устройствам.
				}																											//
			//	Если обнаружен адрес принадлежащий нескольким устройствам:													//
				if( flgDOUBLE ){																							//
					cntDOUBLE--;																							//
				//	Отправляем широковещательным запросом биты занятых адресов в регистры "Coil" DO[0x0101...0x01F7]:		//
					beginTransmission(COILS, 0x0101, 247);																	//	Инициируем передачу данных в регистры "Coil" (DO), начиная с адреса 0x0101, всего 247 регистров.
					for(uint8_t i=1; i<=247; i++){ write( (arrI[i/8]|arrU[i/8])&(1<<(i%8)) ); }								//	Ставим очередной бит в очередь на запись.
					endTransmission();																						//	Выполняем запись.
					delay(10);																								//	Ждём применения значений регистров "Coil" DO[0x0101...0x01F7].
				//	Отправляем широковещательный запрос о временной смене адреса на случайное число:						//
					holdingRegisterWrite(0x0110, 0x0000 ); delay(10);														//	Смена адреса осуществляется при выполнении двух условий: DO[0x0100|ID]==0, где ID - текущий адрес устройства и новый адрес устройства.
					holdingRegisterWrite(0x0100, 0xF0A5 ); delay(10);														//	Подтверждаем смену адреса.
				}																											//
			}																												//
			while( flgDOUBLE && cntDOUBLE );																				//	Повторяем проверку избавления от адресов принадлежащих нескольким устройствам, пока не сбросится cntDOUBLE.
			delay(50);																										//
		//	Возвращаем ошибку если на шине остались адреса принадлежащего нескольким устройствам:							//
			if( flgDOUBLE ){ codeError=ERROR_ADR_IARDUINO; return -1; }														//	
		//	Выполняем сортировку адресов:																					//
			for(uint8_t i=start; i<=247; i++){																				//	Проходим по назначаемым адресам.
			//	Если устройство с адресом i уже присутствует на шине:														//
				if( (arrI[i/8]|arrU[i/8])&(1<<(i%8)) ){																		//
				//	Если устройству iarduino адрес i был назначен временно, то сохраняем его:								//
					if( (arrI[i/8])&(1<<(i%8)) ){ if( coilRead(i,0x0100)==1 ){ if( changeID(i, i) ){ cntCHANGE++; }}}		//	
				//	Исключаем адрес устройства из массива arrI:																//
					arrI[i/8]&=~(1<<(i%8));																					//
			//	Если устройства с адресом i нет на шине:																	//
				}else{																										//
				//	Находим очередное устройство iarduino в массиве arrI:													//
					for(uint8_t j=1; j<=247; j++){ if( (arrI[j/8])&(1<<(j%8)) ){											//
					//	Меняем адрес устройства iarduino с j на i:															//
						if( changeID(j, i) ){ cntCHANGE++; }																//
					//	Исключаем адрес устройства из массива arrI:															//
						arrI[j/8]&=~(1<<(j%8)); j=248;																		//
					}}																										//
				}																											//
			}																												//
			delay(50);																										//
		//	Возвращаем количество устройств которым был изменён адрес:														//
			return cntCHANGE;																								//
}																															//
																															//
//		ФУНКЦИЯ СОХРАНЕНИЯ НОВОГО ID ДЛЯ УСТРОЙСТВА IARDUINO:																//	Функция возвращает результат сохранения адреса.
bool	ModbusClient::changeID(uint8_t nowID, uint8_t newID){																//	nowID - текущий id модуля, newID - новый id модуля.
			if( nowID==0 || newID==0 || nowID>247 || newID>247 ){ codeError=ERROR_GATEWAY_NO_DEVICE; return false; } uint8_t i, j;								//	проверяем данные.
			j=10; do{i=holdingRegisterWrite(nowID,0x0110,0xFF00|newID); delay( 20); j--;} while(!i && j); if(!i){codeError=ERROR_ADR_IARDUINO; return false;}	//	Меняем адрес.
			j=10; do{i=holdingRegisterWrite(newID,0x0100,0xF0A5      ); delay(100); j--;} while(!i && j); if(!i){codeError=ERROR_ADR_IARDUINO; return false;}	//	Подтверждаем изменение.
			return true;																									//
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ОДНОГО РЕГИСТРА "COIL" (DO):																			//	Возвращает прочитанное значение (0/1), или -1 при неудаче.
int8_t	ModbusClient::coilRead(uint8_t id, uint16_t reg){																	//	id - SlaveID, reg - адрес регистра.
			arrADU[0]=id; arrADU[1]=0x01; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=0x00; arrADU[5]=0x01; lenADU=6;	//	Формируем запрос: [0] SlaveID, [1] COM=0x01, [2:3] ardREG, [4:5] sumREG.
			if( _rwADU()==false ){ return -1; }																				//	Выполняем отправку запроса и получения ответа.
			if( lenADU!=4       ){ codeError=ERROR_LEN_RESPONSE; return -1; }												//	Проверяем размер полученного ответа без CRC.
			if( arrADU[2]!=0x01 ){ codeError=ERROR_VAL_RESPONSE; return -1; }												//	Проверяем количество байт данных в полученном ответе.
			return arrADU[3]&0x01;																							//	Возвращаем состояние регистра "Coil" (DO).
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ОДНОГО РЕГИСТРА "DISCRETE INPUT" (DI):																//	Возвращает прочитанное значение (0/1), или -1 при неудаче.
int8_t	ModbusClient::discreteInputRead(uint8_t id, uint16_t reg){															//	id - SlaveID, reg - адрес регистра.
			arrADU[0]=id; arrADU[1]=0x02; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=0x00; arrADU[5]=0x01; lenADU=6;	//	Формируем запрос: [0] SlaveID, [1] COM=0x02, [2:3] ardREG, [4:5] sumREG.
			if( _rwADU()==false ){ return -1; }																				//	Выполняем отправку запроса и получения ответа.
			if( lenADU!=4       ){ codeError=ERROR_LEN_RESPONSE; return -1; }												//	Проверяем размер полученного ответа без CRC.
			if( arrADU[2]!=0x01 ){ codeError=ERROR_VAL_RESPONSE; return -1; }												//	Проверяем количество байт данных в полученном ответе.
			return arrADU[3]&0x01;																							//	Возвращаем состояние регистра "Discrete Input" (DI).
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ОДНОГО РЕГИСТРА "HOLDING REGISTER" (AO):																//	Возвращает прочитанное значение ((0...65535), или -1 при неудаче.
int32_t	ModbusClient::holdingRegisterRead(uint8_t id, uint16_t reg){														//	id - SlaveID, reg - адрес регистра.
			arrADU[0]=id; arrADU[1]=0x03; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=0x00; arrADU[5]=0x01; lenADU=6;	//	Формируем запрос: [0] SlaveID, [1] COM=0x03, [2:3] ardREG, [4:5] sumREG.
			if( _rwADU()==false ){ return -1; }																				//	Выполняем отправку запроса и получения ответа.
			if( lenADU!=5       ){ codeError=ERROR_LEN_RESPONSE; return -1; }												//	Проверяем размер полученного ответа без CRC.
			if( arrADU[2]!=0x02 ){ codeError=ERROR_VAL_RESPONSE; return -1; }												//	Проверяем количество байт данных в полученном ответе.
			return (int32_t)((uint16_t)((arrADU[3]<<8) | arrADU[4]));														//	Возвращаем значение регистра "Holding Register" (AO).
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ОДНОГО РЕГИСТРА "INPUT REGISTER" (AI):																//	Возвращает прочитанное значение ((0...65535), или -1 при неудаче.
int32_t	ModbusClient::inputRegisterRead(uint8_t id, uint16_t reg){															//	id - SlaveID, reg - адрес регистра.
			arrADU[0]=id; arrADU[1]=0x04; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=0x00; arrADU[5]=0x01; lenADU=6;	//	Формируем запрос: [0] SlaveID, [1] COM=0x04, [2:3] ardREG, [4:5] sumREG.
			if( _rwADU()==false ){ return -1; }																				//	Выполняем отправку запроса и получения ответа.
			if( lenADU!=5       ){ codeError=ERROR_LEN_RESPONSE; return -1; }												//	Проверяем размер полученного ответа без CRC.
			if( arrADU[2]!=0x02 ){ codeError=ERROR_VAL_RESPONSE; return -1; }												//	Проверяем количество байт данных в полученном ответе.
			return (int32_t)((uint16_t)((arrADU[3]<<8) | arrADU[4]));														//	Возвращаем значение регистра "Input Register" (AI).
}																															//
																															//
//		ФУНКЦИЯ ЗАПИСИ В ОДИН РЕГИСТР "COIL" (DO):																			//	Возвращает флаг успеха записи (0/1).
bool	ModbusClient::coilWrite(uint8_t id, uint16_t reg, bool val){														//	id - SlaveID, reg - адрес регистра, val - значение (0/1).
			arrADU[0]=id; arrADU[1]=0x05; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=val?0xFF:0x00; arrADU[5]=0x00; lenADU=6;	//	Формируем запрос: [0] SlaveID, [1] COM=0x05, [2:3] ardREG, [4:5] DATA (0000/FF00).
			if(   _rwADU()==false                 ){ return 0; }															//	Выполняем отправку запроса и получения ответа.
			if(   lenADU!=6                       ){ codeError=ERROR_LEN_RESPONSE; return 0; }								//	Проверяем размер полученного ответа без CRC.
			if( ((arrADU[2]<<8)|arrADU[3]) != reg ){ codeError=ERROR_ADR_RESPONSE; return 0; }								//	Проверяем адрес регистра в полученном ответе, он должен совпасть с reg.
			if(  (arrADU[4]!=0xFF) && (val==1)    ){ codeError=ERROR_VAL_RESPONSE; return 0; }								//	Проверяем значение поля DATA в полученном ответе.
			if(  (arrADU[4]!=0x00) && (val==0)    ){ codeError=ERROR_VAL_RESPONSE; return 0; }								//	Проверяем значение поля DATA в полученном ответе.
			if(   arrADU[5] != 0                  ){ codeError=ERROR_VAL_RESPONSE; return 0; }								//	Проверяем значение поля DATA в полученном ответе.
			return 1;																										//	Возвращаем флаг успешной записи в регистр "Coil" (DO).
}																															//
																															//
//		ФУНКЦИЯ ЗАПИСИ В ОДИН РЕГИСТР "HOLDING REGISTER" (AO):																//	Возвращает флаг успеха записи (0/1).
bool	ModbusClient::holdingRegisterWrite(uint8_t id, uint16_t reg, uint16_t val){											//	id - SlaveID, reg - адрес регистра, val - значение (0...65535).
			arrADU[0]=id; arrADU[1]=0x06; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=val>>8; arrADU[5]=val&0xFF; lenADU=6;	//	Формируем запрос: [0] SlaveID, [1] COM=0x06, [2:3] ardREG, [4:5] DATA (0...65535).
			if(   _rwADU()==false                 ){ return 0; }															//	Выполняем отправку запроса и получения ответа.
			if(   lenADU!=6                       ){ codeError=ERROR_LEN_RESPONSE; return 0; }								//	Проверяем размер полученного ответа без CRC.
			if( ((arrADU[2]<<8)|arrADU[3]) != reg ){ codeError=ERROR_ADR_RESPONSE; return 0; }								//	Проверяем адрес регистра в полученном ответе, он должен совпасть с reg.
			if( ((arrADU[4]<<8)|arrADU[5]) != val ){ codeError=ERROR_VAL_RESPONSE; return 0; }								//	Проверяем поле DATA в полученном ответе, оно должно совпадать с val.
			return 1;																										//	Возвращаем флаг успешной записи в регистр "Holding Register" (AO).
}																															//
																															//
//		ФУНКЦИЯ ЗАПИСИ МАСОК В ОДИН РЕГИСТР "HOLDING REGISTER" (AO):														//	Возвращает флаг успеха записи (0/1).
bool	ModbusClient::registerMaskWrite(uint8_t id, uint16_t reg, uint16_t a, uint16_t o){									//	id - SlaveID, reg - адрес регистра, a - маска AND, o - маска OR.
			arrADU[0]=id; arrADU[1]=0x16; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=a>>8; arrADU[5]=a&0xFF; arrADU[6]=o>>8; arrADU[7]=o&0xFF; lenADU=8;	//	Формируем запрос: [0] SlaveID, [1] COM=0x06, [2:3] ardREG, [4:5] AND, [6:7] OR.
			if(   _rwADU()==false                 ){ return 0; }															//	Выполняем отправку запроса и получения ответа.
			if(   lenADU!=8                       ){ codeError=ERROR_LEN_RESPONSE; return 0; }								//	Проверяем размер полученного ответа без CRC.
			if( ((arrADU[2]<<8)|arrADU[3]) != reg ){ codeError=ERROR_ADR_RESPONSE; return 0; }								//	Проверяем адрес регистра в полученном ответе, он должен совпасть с reg.
			if( ((arrADU[4]<<8)|arrADU[5]) != a   ){ codeError=ERROR_VAL_RESPONSE; return 0; }								//	Проверяем поле AND в полученном ответе, оно должно совпадать с a.
			if( ((arrADU[6]<<8)|arrADU[7]) != o   ){ codeError=ERROR_VAL_RESPONSE; return 0; }								//	Проверяем поле OR  в полученном ответе, оно должно совпадать с o.
			return 1;																										//	Возвращаем флаг успешной записи в регистр "Holding Register" (AO).
}																															//
																															//
//		ФУНКЦИЯ ИНИЦИАЛИЗАЦИИ ЗАПИСИ В НЕСКОЛЬКО РЕГИСТРОВ (DO) ИЛИ (AO):													//	Возвращает флаг успеха инициализации (0/1).
bool	ModbusClient::beginTransmission(uint8_t id, uint8_t type, uint16_t reg, uint16_t n){								//	id - SlaveID, type - тип регистра (COILS/HOLDING_REGISTERS), reg - адрес первого регистра, n - количество регистров.
			if( (type!=COILS) && (type!=HOLDING_REGISTERS) ){ codeError=ERROR_SYNTAX; return 0; }							//	Возвращаем флаг ошибки.
			uint16_t sumBYTE=(type==COILS)?((n/8)+(n%8?1:0)):(n*2); if( sumBYTE>247 ){ codeError=ERROR_LEN_REQUEST; return 0; }	//	Определяем количество байт для отправки значений в n регистров.
			arrADU[0]=id; arrADU[1]=(type==COILS)?0x0F:0x10; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=n>>8; arrADU[5]=n&0xFF; arrADU[6]=sumBYTE&0xFF; for(uint8_t i=0; i<sumBYTE; i++){arrADU[7+i]=0;} lenADU=7+sumBYTE; // Формируем часть запроса: [0] SlaveID, [1] COM=0x0F/0x10, [2:3] ardREG, [4:5] sumREG, [6] sumBYTE.
			transmissionFLG=true;																							//	Устанавливаем флаг инициализации записи в несколько регистров.
			transmissionCNT=0;																								//	Сбрасываем счётчик вызова функций write().
			return 1;																										//	Возвращаем флаг успешной записи в регистр "Holding Register" (AO).
}																															//
																															//
//		ФУНКЦИЯ ПОСТАНОВКИ ЗНАЧЕНИЯ В ОЧЕРЕДЬ НА ЗАПИСЬ В НЕСКОЛЬКО РЕГИСТРОВ (DO) ИЛИ (AO):								//	Возвращает флаг успеха постановки значения в очередь (0/1).
bool	ModbusClient::write(uint16_t val){																					//	val - очередное значение для записи.
			if( transmissionFLG==false                      ){ codeError=ERROR_SYNTAX;        return 0; }					//	Возвращаем флаг ошибки.
			if( transmissionCNT>=((arrADU[4]<<8)|arrADU[5]) ){ codeError=ERROR_ILLEGAL_VALUE; return 0; }					//	Возвращаем флаг ошибки.
			if( arrADU[1]==0x0F ){ arrADU[7+(transmissionCNT/8)]|=((val?1:0)<<(transmissionCNT%8)); }						//	Добавляем бит для записи в регистр "Coil" (DO).
			else{ arrADU[7+(transmissionCNT*2)]=val>>8; arrADU[8+(transmissionCNT*2)]=val&0xFF; }							//	Добавляем 2 байта для записи в регистр "Holding Register" (AO).
			transmissionCNT++;																								//
			return 1;																										//	Возвращаем флаг успешной постановки значения в очередь (0/1).
}																															//
																															//
//		ФУНКЦИЯ ВЫПОЛНЕНИЯ ЗАПИСИ В НЕСКОЛЬКО РЕГИСТРОВ (DO) ИЛИ (AO):														//	Возвращает флаг успеха записи (0/1).
bool	ModbusClient::endTransmission(void){																				//
			uint16_t reg = (arrADU[2]<<8)|arrADU[3];																		//	Получаем адрес первого регистра для записи.
			uint16_t n   = (arrADU[4]<<8)|arrADU[5];																		//	Получаем количество записываемых регистров.
			if( transmissionFLG==false            ){ codeError=ERROR_SYNTAX;       return 0; }								//	Возвращаем флаг ошибки.
			if(  _rwADU()==false                  ){                               return 0; }								//	Выполняем отправку запроса и получения ответа.
			if(   lenADU!=6                       ){ codeError=ERROR_LEN_RESPONSE; return 0; }								//	Проверяем размер полученного ответа без CRC.
			if( ((arrADU[2]<<8)|arrADU[3]) != reg ){ codeError=ERROR_ADR_RESPONSE; return 0; }								//	Проверяем адрес регистра в полученном ответе, он должен совпасть с reg.
			if( ((arrADU[4]<<8)|arrADU[5]) != n   ){ codeError=ERROR_VAL_RESPONSE; return 0; }								//	Проверяем количество записанных регистров, оно должно совпасть с n.
			return 1;																										//	Возвращаем флаг успешной записи в несколько регистров "Coils" (DO), или "Holding Registers" (AO).
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ИЗ НЕСКОЛЬКИХ РЕГИСТРОВ (DO), (DI), (AO), (AI):														//	Возвращает количество прочитанных значений, или 0 при неудаче.
uint16_t ModbusClient::requestFrom(uint8_t id, uint8_t type, uint16_t reg, uint16_t n){										//	id - SlaveID, type - тип регистра (COILS/DISCRETE_INPUTS/HOLDING_REGISTERS/INPUT_REGISTERS), reg - адрес первого регистра, n - количество регистров.
			if( type==COILS             ){ arrADU[1]=0x01; }else															//	Выполняем команду чтения из нескольких регистров "Coils" (DO).
			if( type==DISCRETE_INPUTS   ){ arrADU[1]=0x02; }else															//	Выполняем команду чтения из нескольких регистров "Discrete Inputs" (DI).
			if( type==HOLDING_REGISTERS ){ arrADU[1]=0x03; }else															//	Выполняем команду чтения из нескольких регистров "Holding Registers" (AO).
			if( type==INPUT_REGISTERS   ){ arrADU[1]=0x04; }else{ codeError=ERROR_SYNTAX; return 0; }						//	Выполняем команду чтения из нескольких регистров "Input Registers" (AI).
			arrADU[0]=id; arrADU[2]=reg>>8; arrADU[3]=reg&0xFF; arrADU[4]=n>>8; arrADU[5]=n&0xFF; lenADU=6;					//	Формируем запрос: [0] SlaveID, [1] COM (0x01...0x04), [2:3] ardREG, [4:5] sumREG.
			if( _rwADU()==false ){ return 0; }																				//	Выполняем отправку запроса и получения ответа.
			if( lenADU<3        ){ codeError=ERROR_LEN_RESPONSE; return 0; }												//	Проверяем размер полученного ответа без CRC.
			if( arrADU[1]<=0x02 ){ requestSUM=arrADU[2]*8; if(n%8){requestSUM-=(8-(n%8)); } }								//	Определяем количество прочитанных значений регистров (DO), или (DI) доступных для чтения функцией read().
			if( arrADU[1]>=0x03 ){ requestSUM=arrADU[2]/2; }																//	Определяем количество прочитанных значений регистров (AO), или (AI)
			requestCNT=requestSUM;																							//	Определяем количество значений доступных для чтения функцией read().
			return requestSUM;																								//	Возвращаем количество прочитанных значений.
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ОЧЕРЕДНОГО ПОЛУЧЕННОГО ЗНАЧЕНИЯ ПОСЛЕ requestFrom(), findID(), getEventLog(), getInfo():				//	Возвращает значение (0/1/0...65535), или -1 при неудаче.
int32_t ModbusClient::read(void){																							//
			uint16_t i, j=requestSUM-requestCNT;																			//
			if( requestCNT==0   ){ codeError=ERROR_SYNTAX; return -1; }														//	Возвращаем флаг ошибки.
			if( arrADU[1]<=0x02 ){ i=(arrADU[3+(j/8)]&(1<<(j%8)))?1:0;    }else												//	Читаем бит полученный из регистров (DO), или (DI).
			if( arrADU[1]<=0x04 ){ i=(arrADU[3+(j*2)]<<8)|arrADU[4+(j*2)];}else												//	Читаем 2 байта полученные из регистров (AO), или (AI).
			if( arrADU[1]==0x05 ){ i= arrADU[2+ j   ]; }else																//	Читаем 1 байт найденный функцией findID().
			if( arrADU[1]==0x11 ){ i= arrADU[3+ j   ]; }else																//	Читаем 1 байт полученный функцией getInfo().
			if( arrADU[1]==0x0C ){ if(j<3){i=(arrADU[3+(j*2)]<<8)|arrADU[4+(j*2)];}else{i=arrADU[6+j];} }else				//	Читаем 2 или 1 байт полученный функцией getEventLog().
			                     { codeError=ERROR_SYNTAX; return -1; }														//	
			requestCNT--;																									//	Уменьшаем счётчик количества оставшихся обращений к функций read()
			return (int32_t)i;																								//	Возвращаем очередное значение.
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ОЧЕРЕДНОГО ПОЛУЧЕННОГО ЗНАЧЕНИЯ ПОСЛЕ REQUESTFROM():													//	Возвращает значение (0/1/0...65535), или -1 при неудаче.
void	ModbusClient::end(void){																							//
			if( pinDE<0xFF ){ pinMode(pinDE, INPUT); }																		//	Освобождаем вывод pinDE.
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ СОСТОЯНИЯ 8 СТАТУСНЫХ ВЫХОДОВ:																		//	Возвращает байт битов, или -1 при неудаче.
int16_t	ModbusClient::exceptionStatusRead(uint8_t id){																		//	id - SlaveID.
			arrADU[0]=id; arrADU[1]=0x07; lenADU=2;																			//	Формируем запрос: [0] SlaveID, [1] COM=0x07.
			if( _rwADU()==false ){ return -1; }																				//	Выполняем отправку запроса и получения ответа.
			if( lenADU!=3       ){ codeError=ERROR_LEN_RESPONSE; return -1; }												//	Проверяем размер полученного ответа без CRC.
			return arrADU[2];																								//	Возвращаем байт битов состояния статусных выходов.
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ СОСТОЯНИЯ УСТРОЙСТВА:																				//	Возвращает бит состояния (0-свободно,1-занято), или -1 при неудаче.
int8_t ModbusClient::getSate(uint8_t id){																					//	id - SlaveID.
			arrADU[0]=id; arrADU[1]=0x0B; lenADU=2;																			//	Формируем запрос: [0] SlaveID, [1] COM=0x0B.
			if( _rwADU()==false ){ return -1; }																				//	Выполняем отправку запроса и получения ответа.
			if( lenADU!=6       ){ codeError=ERROR_LEN_RESPONSE; return -1; }												//	Проверяем размер полученного ответа без CRC.
			return (((arrADU[2]==0)&&(arrADU[3]==0))?0:1);																	//	Возвращаем бит состояния (0-свободно,1-занято).
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ СЧЕТЧИКА СОБЫТИЙ (УСПЕШНО ВЫПОЛНЕНЫХ ЗАПРОСОВ):														//	Возвращает значение счётчика событий, или -1 при неудаче.
int32_t ModbusClient::getEventCounter(uint8_t id){																			//	id - SlaveID.
			arrADU[0]=id; arrADU[1]=0x0B; lenADU=2;																			//	Формируем запрос: [0] SlaveID, [1] COM=0x0B.
			if( _rwADU()==false ){ return -1; }																				//	Выполняем отправку запроса и получения ответа.
			if( lenADU!=6       ){ codeError=ERROR_LEN_RESPONSE; return -1; }												//	Проверяем размер полученного ответа без CRC.
			return (int32_t)((uint16_t)((arrADU[4]<<8) | arrADU[5]));														//	Возвращаем значение счётчика событий.
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ЖУРНАЛА СОБЫТИЙ:																						//	Возвращает количество данных, доступных для чтения функцией read() и available().
uint8_t	ModbusClient::getEventLog(uint8_t id){																				//	id - SlaveID.
			arrADU[0]=id; arrADU[1]=0x0C; lenADU=2;																			//	Формируем запрос: [0] SlaveID, [1] COM=0x0C.
			if( _rwADU()==false       ){ return 0; }																		//	Выполняем отправку запроса и получения ответа.
			if( lenADU<3              ){ codeError=ERROR_LEN_RESPONSE; return 0; }											//	Проверяем размер полученного ответа без CRC.
			if( lenADU!=(arrADU[2]+3) ){ codeError=ERROR_LEN_RESPONSE; return 0; }											//	Проверяем размер полученного ответа без CRC.
			if( arrADU[2]<6           ){ codeError=ERROR_VAL_RESPONSE; return 0; }											//	Проверяем количество байт в полях STATE[2], EventCNT[2], QueryCNT[2].
			requestSUM=arrADU[2]-3; requestCNT=requestSUM; return requestSUM;												//	Сохраняем количество данных (STATE, EventCNT, QueryCNT, EVENTS[]) как доступные для чтения функцией read().
}																															//
																															//
//		ФУНКЦИЯ ВЫПОЛНЕНИЯ КОМАНДЫ ДИАГНОСТИКИ:																				//	Возвращает данные результата выполнения функции диагностики, или -1 при неудаче.
int32_t ModbusClient::diagnostic(uint8_t id, uint16_t func, uint16_t data){													//	id - SlaveID, func - номер функции диагностики, data - данные.
			arrADU[0]=id; arrADU[1]=0x08; arrADU[2]=func>>8; arrADU[3]=func&0xFF; arrADU[4]=data>>8; arrADU[5]=data&0xFF; lenADU=6;	//	Формируем запрос: [0] SlaveID, [1] COM=0x08, [2:3] func, [4:5] data.
			if( _rwADU()==false                    ){ return -1; }															//	Выполняем отправку запроса и получения ответа.
			if( lenADU!=6                          ){ codeError=ERROR_LEN_RESPONSE; return -1; }							//	Проверяем размер полученного ответа без CRC.
			if( ((arrADU[2]<<8)|arrADU[3]) != func ){ codeError=ERROR_VAL_RESPONSE; return -1; }							//	Проверяем номер функции в ответе, он должен совпасть с func.
			return (int32_t)((uint16_t)((arrADU[4]<<8) | arrADU[5]));														//	Возвращаем значение поля DATA.
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ИНФОРМАЦИИ ОБ УСТРОЙСТВЕ:																			//	Возвращает количество байт данных информации об устройстве, доступных для чтения функцией read() и available().
uint8_t	ModbusClient::getInfo(uint8_t id){																					//	id - SlaveID.
			arrADU[0]=id; arrADU[1]=0x11; lenADU=2;																			//	Формируем запрос: [0] SlaveID, [1] COM=0x11.
			if( _rwADU()==false       ){ return 0; }																		//	Выполняем отправку запроса и получения ответа.
			if( lenADU<5              ){ codeError=ERROR_LEN_RESPONSE; return 0; }											//	Проверяем размер полученного ответа без CRC.
			if( lenADU!=(arrADU[2]+3) ){ codeError=ERROR_LEN_RESPONSE; return 0; }											//	Проверяем размер полученного ответа без CRC.
			if( arrADU[2]<2           ){ codeError=ERROR_VAL_RESPONSE; return 0; }											//	Проверяем количество байт в полях ID*, WORK, DATA.
			requestSUM=arrADU[2]; requestCNT=requestSUM; return requestSUM;													//	Сохраняем количество байт в полях ID*, WORK, DATA как доступные для чтения функцией read().
}																															//
																															//
//		ФУНКЦИЯ ОТПРАВКИ/ПОЛУЧЕНИЯ ADU:																						//
bool	ModbusClient::_rwADU(void){																							//	Функция отправки/получения данных из/в массив arrADU длиной lenADU байт.
			transmissionFLG=false;																							//	Сбрасываем флаг инициализации записи в несколько регистров.
			requestCNT=0;																									//	Сбрасываем счётчик количества оставшихся обращений к функций read().
		//	Сохраняем значения SlaveID и Command для сравнения с ответом:													//
			uint8_t SlaveID=arrADU[0];																						//
			uint8_t Command=arrADU[1];																						//
			if( Command&0x80 ){ codeError=ERROR_ILLEGAL_FUNCTION; return false; }											//	Ошибка: некорректная команда.
		//	Добавляем CRC:																									//
			_crc16(lenADU); arrADU[lenADU++]=valCRC&0x00FF; arrADU[lenADU++]=valCRC>>8;										//
		//	Если используется протокол ModBus ASCII.																		//
			if( typeModbus==MODBUS_ASCII ){																					//	Если используется протокол ModBus ASCII.
			//	Проверяем размер пакета ADU в ModBus ASCII:																	//
				if( (lenADU*2+3)>=MODBUS_ADU_MAX ){ codeError=ERROR_LEN_REQUEST; return false; }							//	Ошибка: превышен размер буфера.
			//	ПРЕОБРАЗУЕМ пакет ADU из ModBus RTU в ModBus ASCII.															//
				for(uint16_t i=lenADU; i; i--){																				//
					arrADU[i*2  ]=_char(arrADU[i-1]&0x0F);																	//	Сохраняем символ старшего полубайта.
					arrADU[i*2-1]=_char(arrADU[i-1]>>4  );																	//	Сохраняем символ младшего полубайта.
				}	arrADU[0    ]=':';																						//	Сохраняем первый символ пакета ADU.
					lenADU*=2;																								//	Сохраняем количество символов данных пакета ADU.
					lenADU+=3;																								//	Добавляем количество символов начала (:) и конца (\r\n) пакета.
					arrADU[lenADU-1]='\r';																					//	Сохраняем предпоследний символ пакета ADU.
					arrADU[lenADU-2]=charASCII;																				//	Сохраняем последний символ пакета ADU MODBUS_ASCII.
			}																												//
		//	Чистим буфер приёма данных UART:																				//
			while( objSerial->available() ){ objSerial->read(); }															//
		//	Ждём интервал valTimeoutSET до начала отправки данныx:															//
			if( codeError!=ERROR_GATEWAY_NO_DEVICE ){ while( _waitTo(valTimeoutSET) ); }									//
		//	Разрешаем работу передатчика RS485:																				//
			if( pinDE<0xFF ){ digitalWrite(pinDE, HIGH); }																	//	Устанавливаем на выводе pinDE уровень логической 1.
		//	Отправляем данные:																								//
			objSerial->write(arrADU, lenADU);																				//	Отправляем массив arrADU из lenADU байт.
			objSerial->flush();																								//	Ждём окончания передачи данных.
		//	Запрещаем работу передатчика RS485:																				//
			if( pinDE<0xFF ){ digitalWrite(pinDE, LOW); }																	//	Устанавливаем на выводе pinDE уровень логического 0.
		//	Получаем ответ:																									//
			lenADU=0;																										//
		//	Ждём интервал valTimeoutGET до приёма ответа:																	//
			timEndPack=millis(); while( _waitTo(valTimeoutGET) && !objSerial->available() );								//	Ждём появления первого байта, но не более valTimeoutGET мс.
			if( !objSerial->available() ){ codeError=ERROR_GATEWAY_NO_DEVICE; return false; }								//	Ошибка: отсутствие ответа.
		//	Если используется протокол ModBus RTU.																			//
			if( typeModbus==MODBUS_RTU ){																					//	Если используется протокол ModBus RTU.
			//	Ждём получения очередного байта:																			//
				timEndPack=millis();																						//
				while( _waitTo(valTimeoutGET/2) ){																			//	Ждём появления очередного байта, но не более 1/2 valTimeoutGET мс.
					if( objSerial->available() ){																			//
						timEndPack=millis();																				//
						arrADU[lenADU++]=objSerial->read();																	//	Сохраняем очередной байт в массив arrADU.
						if( lenADU>=MODBUS_ADU_MAX ){ codeError=ERROR_LEN_RESPONSE; return false; }							//	Ошибка: превышен размер буфера.
					}																										//
				}																											//
			}																												//
		//	Если используется протокол ModBus ASCII.																		//
			if( typeModbus==MODBUS_ASCII ){																					//	Если используется протокол ModBus ASCII.
				bool    start=0, stop=0;																					//
				char    a[2]={0,0};																							//
				uint8_t i=0;																								//
			//	Ждём получения очередного символа:																			//
				timEndPack=millis();																						//
				while( _waitTo(valTimeoutGET/2) && !stop ){																	//	Ждём появления очередного символа, но не более 1/2 valTimeoutGET мс.
					if( objSerial->available() ){																			//
						timEndPack=millis();																				//
						a[i++]=objSerial->read();																			//	Читаем очередной символ в a[0] или a[1].
						if( !start ){																						//
						//	Ждём наладо пакета:																				//
							i=0; if( a[i]==':' ){ start=1; }																//	Отслеживаем появление символа начала пакета ':', тогда устанавливаем флаг start.
						}else{																								//
						//	Получаем данные:																				//
							if( a[(i-1)]=='\r' || a[(i-1)]==charASCII ){ i=0; stop=1;}										//	Отслеживаем появление символа конца пакета '\r' или charASCII, тогда устанавливаем флаг stop.
							if( i>=2 ){ i=0; arrADU[lenADU++]=(_num(a[0])<<4)|_num(a[1]); }									//	Каждые 2 символа преобразуем в байт и сохраняем в массив arrADU.
							if( lenADU>=MODBUS_ADU_MAX ){ codeError=ERROR_LEN_RESPONSE; return false; }						//	Ошибка: превышен размер буфера.
						}																									//
					}																										//
				}																											//
			}																												//
			objSerial->flush();																								//	Ждём окончания передачи данных.
			timEndPack=millis();																							//
		//	Проверяем ответ:																								//
			_crc16(lenADU-2);																								//	Вычисляем CRC полученного пакета без двух последний байт полученного CRC.
			if( lenADU==0                        ){ codeError=ERROR_GATEWAY_NO_DEVICE;   return false; }					//	Ошибка: отсутствие ответа.
			if( arrADU[0]       != SlaveID       ){ codeError=ERROR_GATEWAY_UNAVAILABLE; return false; }					//	Ошибка: несовпадение SlaveID, шлюз перегружен запросами.
			if((arrADU[1]&0x7F) != Command       ){ codeError=ERROR_GATEWAY_UNAVAILABLE; return false; }					//	Ошибка: несовпадение Command, шлюз перегружен запросами.
			if( arrADU[--lenADU]!=(valCRC>>8)    ){ codeError=ERROR_CRC_RESPONSE;        return false; }					//	Ошибка: несовпадение старшего байта CRC.
			if( arrADU[--lenADU]!=(valCRC&0x00FF)){ codeError=ERROR_CRC_RESPONSE;        return false; }					//	Ошибка: несовпадение младшего байта CRC.
			if( arrADU[1]&0x80                   ){ codeError=arrADU[2];                 return false; }					//	Ошибка: модуль вернул сообщение об ошибке.
		//	Возвращаем флаг успеха приёма/передачи:																			//
			return true;																									//
}																															//
																															//
//		ФУНКЦИЯ ПРЕОБРАЗОВАНИЯ СИМВОЛА В ЧИСЛО:																				//	Функция возвращает:	число uint8_t.
uint8_t	ModbusClient::_num(char symbol){																					//	Аргументы функции:	символ 0-9,a-f,A-F.
			uint8_t i = uint8_t(symbol);																					//	Получаем код символа
			if ( (i>='0') && (i<='9') )	{ return i-'0';    }else															//	0-9
			if ( (i>='A') && (i<='F') )	{ return i-'A'+10; }else															//	A-F
			if ( (i>='a') && (i<='f') )	{ return i-'a'+10; }else															//	a-f
										{ return 0;        }																//	остальные символы вернут число 0.
}																															//
																															//
//		ФУНКЦИЯ ПРЕОБРАЗОВАНИЯ ЧИСЛА В СИМВОЛ:																				//	Функция возвращает:	символ char.
char	ModbusClient::_char(uint8_t num){																					//	Аргументы функции:	число 0-15.
			if(num<10){return char(num+'0'   );}else																		//	0-9
			if(num<16){return char(num+'A'-10);}else																		//	A-F
			          {return '0';}																							//
}																															//
																															//
//		ФУНКЦИЯ РАССЧЁТА CRC-16:																							//
void	ModbusClient::_crc16(uint8_t len){																					//	Параметры: количество байт в arrADU. Результат сохраняется в valCRC.
			valCRC=0xFFFF;																									//	Опреджеляем начальное значение crc.
			for(uint8_t i=0; i<len; i++){																					//	Проходим по len элементам массива arr.
				valCRC^=arrADU[i]; for(uint8_t j=0; j<8; j++){if(valCRC&0x0001){valCRC>>=1; valCRC^=0xA001;}else{valCRC>>=1;}}	//	Вычисляем CRC-16.
			}																												//
}																															//
																															//
//		ФУНКЦИЯ ОЖИДАНИЯ ВРЕМЕНИ:																							//
bool	ModbusClient::_waitTo(uint32_t ms){																					//	Параметры: время в миллисекундах
			return ((millis()-timEndPack) < ms) ? 1:0;																		//	Возвращаем true если с момента timEndPack прошло меньше ms миллисекунд.
}																															//
//		ФУНКЦИЯ ПРОВЕРКИ УСТРОЙСТВ:																							//	Функция возвращает:	DEVICE_MB_ABSENT/DEVICE_MB_DOUBLE/DEVICE_MB_UNKNOWN/DEVICE_MB_IARDUINO
uint8_t	ModbusClient::checkID(uint8_t id){																					//	Аргументы функции:	SlaveID 1-255.
			uint8_t i=3;																									//	Количество проверок:
			while( i ){ i--;																								//
				int32_t j=holdingRegisterRead(id,0x0110);																	//	Читаем регистр AO[0x0110]
				if( j<0 ){																									//
				//	Регистр AO[0x0110] не прочитан:																			//
					if( codeError==ERROR_GATEWAY_NO_DEVICE ){ return DEVICE_MB_ABSENT;  }									//	Если код последней ошибки ERROR_GATEWAY_NO_DEVICE, значит устройства нет на шине.
					if( codeError==ERROR_ILLEGAL_FUNCTION  ){ return DEVICE_MB_UNKNOWN; }									//	Если код последней ошибки ERROR_ILLEGAL_FUNCTION , значит это стороннее устройство.
					if( codeError==ERROR_ILLEGAL_ADDRESS   ){ return DEVICE_MB_UNKNOWN; }									//	Если код последней ошибки ERROR_ILLEGAL_ADDRESS  , значит это стороннее устройство.
					if( codeError==ERROR_ILLEGAL_VALUE     ){ return DEVICE_MB_UNKNOWN; }									//	Если код последней ошибки ERROR_ILLEGAL_VALUE    , значит это стороннее устройство.
				}else{																										//
				//	Регистр AO[0x0110] прочитан:																			//
					if( (j&0x000000FFL)!=id                ){ return DEVICE_MB_UNKNOWN; }else								//	Если младший байт полученного значения не совпадает с адресом, значит это стороннее устройство.
					{																										//	Иначе.
					//	Проверка на принадлежность к линейе iarduino.														//
						if( getInfo(id)                    >=  15  ){														//	Устройство вернуло меньше 15 или более байт данных.
						if( read()                         == 0x77 ){														//	Идентификатор линейки устройств соответствует устройствам iArduino.
							read();																							//	Индикатор пуска не проверяем (00=OFF, FF=ON).
						if( read()                         == id   ){														//	Адрес полученный из информации об устройстве совпадает с фактическим адресом устройства.
						if( holdingRegisterRead(id,0x0110) >=0     ){														//	Повторное читение регистра AO[0x0110] не вызвало ошибок.
						//	Устройство принадлежит линейе iarduino.															//
							return DEVICE_MB_IARDUINO;																		//
						}}}}																								//
						if( codeError==ERROR_ILLEGAL_FUNCTION  ){ return DEVICE_MB_UNKNOWN; }								//	Если код последней ошибки ERROR_ILLEGAL_FUNCTION , значит это стороннее устройство.
						if( codeError==ERROR_ILLEGAL_ADDRESS   ){ return DEVICE_MB_UNKNOWN; }								//	Если код последней ошибки ERROR_ILLEGAL_ADDRESS  , значит это стороннее устройство.
						if( codeError==ERROR_ILLEGAL_VALUE     ){ return DEVICE_MB_UNKNOWN; }								//	Если код последней ошибки ERROR_ILLEGAL_VALUE    , значит это стороннее устройство.
					}																										//
				}																											//
			}																												//
			return DEVICE_MB_DOUBLE;																						//	Если в течении i проверок устройство отвечало но не определено, значит отвечают два устройства.
}																															//
