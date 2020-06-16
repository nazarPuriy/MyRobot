/*
 * main.h
 *
 *  Created on: 19 mar. 2020
 *      Author: droma
 */

#ifndef MAIN_H_
#define MAIN_H_

/* Set project wide debug level
 *   1.
 *   2. Store movement positions
 *   3. Print distance information
 *   4.
 */
#define DEBUG_LEVEL 3
void acelerate(bool more,int quantity);
void findWall();
void adhere(uint8_t center,uint8_t right,uint8_t left,int8_t centerAnt,uint8_t rightAnt,uint8_t leftAnt,uint16_t motorl,uint16_t motorr);
void move();
void findBestDirection();
void a();
#endif /* MAIN_H_ */
