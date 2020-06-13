//
// Created by Nazar on 21/04/2020.
//

#ifndef JOYSTICK_DYN_APP_MOTORS_H
#define JOYSTICK_DYN_APP_MOTORS_H

#include "dyn_instr.h"
#include <stdint.h>

int robot_move_straight(uint16_t speed, bool direction);
int robot_move_left(uint16_t speed, bool direction);
int robot_move_right(uint16_t speed, bool direction);
int robot_rotate(uint16_t speed, bool direction);
int dyn_continous(uint8_t id);
int dyn_move(uint8_t id, uint16_t speed, bool direction);
int subSpeed(uint8_t id);
int addSpeed(uint8_t id);
int changeSpeed(uint8_t id, bool more, uint16_t speed);
int dyn_motor_read_speed(uint8_t id, uint16_t *val, bool *val2);
bool chk_id_motors(uint8_t id);
#endif //JOYSTICK_DYN_APP_MOTORS_H


