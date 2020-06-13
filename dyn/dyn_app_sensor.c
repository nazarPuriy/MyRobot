/*
 * dyn_sensor.c
 *
 *  Created on: 18 mar. 2020
 *      Author: droma
 *
 */

#include "dyn_app_sensor.h"


/**
 * Read all distances.
 *
 * @param[in] id Id of the dynamixel moduel
 * @param[out] 0 if there is no error. Other if there is.
 */
int dyn_sensor_obstacles(uint8_t id,uint8_t *valLeft, uint8_t *valCenter, uint8_t *valRight ){
    int resposta;
    resposta = dyn_sensor_read_left(id,valLeft);
    resposta += dyn_sensor_read_center(id,valCenter);
    resposta += dyn_sensor_read_right(id,valRight);
    return resposta;
}


/**
 * Read the current left sensor status.
 *
 * @param[in] id Id of the dynamixel moduel
 * @param[out] 0 if there is no error. Other if there is.
 */
int dyn_sensor_read_left(uint8_t id, uint8_t *val) {
    if(!chk_id_sensor(id)){return 1;}

    return dyn_read_byte(id, DYN_REG__IR_LEFT, val);
}

/**
 * Read the current right sensor status.
 *
 * @param[in] id Id of the dynamixel moduel
 * @param[out] 0 if there is no error. Other if there is.
 */
int dyn_sensor_read_right(uint8_t id, uint8_t *val) {
    if(!chk_id_sensor(id)){return 1;}

    return dyn_read_byte(id, DYN_REG__IR_RIGHT, val);
}

/**
 * Read the current right sensor status.
 *
 * @param[in] id Id of the dynamixel moduel
 * @param[out] 0 if there is no error. Other if there is.
 */
int dyn_sensor_read_center(uint8_t id, uint8_t *val) {
    if(!chk_id_sensor(id)){return 1;}

    return dyn_read_byte(id, DYN_REG__IR_CENTER, val);
}

/**
 * Checks if the given id corresponds to the sensor id
 *
 * @param[in] id Id of the dynamixel moduel
 * @param[out] true if the given id is correct.
 */
bool chk_id_sensor(uint8_t id){
    if(id==DYN_ID_SENSOR){
        return true;
    }else{
        return false;
    }
}




