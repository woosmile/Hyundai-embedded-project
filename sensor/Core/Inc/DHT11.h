/*
 * DHT.h
 *
 *  Created on: 2024.12.13
 *      Author: sjmun
 */

#ifndef DHT_H_
#define DHT_H_

#define DHTLIB_OK               0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2

typedef struct
{
	float Temperature;
	float Humidity;
}DHT_DataTypedef;


int DHT_GetData (DHT_DataTypedef *DHT_Data);

#endif /* INC_DHT_H_ */
