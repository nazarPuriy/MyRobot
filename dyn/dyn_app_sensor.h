/*
 * dyn_sensor.h
 *
 *  Created on: 18 mar. 2020
 *      Author: droma
 */

#ifndef DYN_SENSOR_H_
#define DYN_SENSOR_H_

#include "dyn_instr.h"
#include <stdint.h>

int dyn_sensor_obstacles(uint8_t id,uint8_t *valLeft, uint8_t *valCenter, uint8_t *valRight );
int dyn_sensor_read_left(uint8_t id, uint8_t *val);
int dyn_sensor_read_center(uint8_t id, uint8_t *val);
int dyn_sensor_read_right(uint8_t id, uint8_t *val);
bool chk_id_sensor(uint8_t id);


#endif /* DYN_SENSOR_H_ */
