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
void adhere();
void littleLeft();
void littleRight();
void move();
void a();
#endif /* MAIN_H_ */
