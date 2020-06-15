//
// Created by Nazar on 21/04/2020.
//
#include "dyn_app_motors.h"

/**
 * OBSERVACIÍO
 * Suposem que el módul 2 (dreta) està col·locat de tal forma que quan gira en sentit del rellotge ens movem endavant.
 * Per tant l'esquerra es mou endavant (el robot) quan girem en sentit invers al rellotge.
 */
uint16_t VEL_MAX = 1023; //Velocitat màxima deguda a que utilitzem 10 bits per declarar aquesta.
uint8_t VEL_MIN = 1; //Velocitat mínima per no entrar en el mode defiinit a 0.
uint8_t VEL_CHANGE=10;

/**
 * Turn on a continous moving of the robot straight ahead or backwards
 *
 * @param[in] speed Defines the velocity of the motors
 * @param[in] direction true means straight ahead, false means backwards
 */
int robot_move_straight(uint16_t speed, bool direction) {
   return dyn_move(DYN_ID_MOTORL,speed,direction) + dyn_move(DYN_ID_MOTORR,speed,direction);}


/**
 * Turn on a continous movement straight/backwards right
 *
 * @param[in] speed Defines the velocity of the external motor
 * @param[in] direction true means straight ahead, false means backwards
 */
int robot_move_right(uint16_t speed, bool direction) {
    //Per tal de moure's cap a la dreta el motor dret ha de girar més lent que l'esquerra.
    return dyn_move(DYN_ID_MOTORL,speed,direction) + dyn_move(DYN_ID_MOTORR,speed/2,direction);
}

/**
 * Turn on a continous movement straight/backwards left
 *
 * @param[in] speed Defines the velocity of the external motor
 * @param[in] direction true means straight ahead, false means backwards
 */
int robot_move_left(uint16_t speed, bool direction) {
    //Per tal de moure's cap a l'esquerra el motor esquerra ha de girar més lent que l'esquerra.
    return dyn_move(DYN_ID_MOTORL,speed/2,direction) + dyn_move(DYN_ID_MOTORR,speed,direction);
}

/**
 * Turn on a continous movement rotating to the direction given.
 *
 * @param[in] speed Defines the velocity of the motors
 * @param[in] direction true makes the robot rotate on clock wise false in clock wise.
 */
 int robot_rotate(uint16_t speed, bool direction){
    //Els motors han de tenir direcció oposada per girar.
    return dyn_move(DYN_ID_MOTORL,speed,direction) + dyn_move(DYN_ID_MOTORR,speed,!direction);

}

 /**
  * Adds speed to the specified motor if needed changes the rotation.
  *
 * @param [in] id Id of the dynamixel moduel
  */
 int subSpeed(uint8_t id){
     if(!chk_id_motors(id)){return 1;}
     return changeSpeed(id,true,VEL_CHANGE);
 }

/**
* Reduces speed to the specified motor if needed changes the rotation.
*
* @param [in] id Id of the dynamixel moduel
*/
int addSpeed(uint8_t id){
    if(!chk_id_motors(id)){return 1;}
    return changeSpeed(id,false,VEL_CHANGE);
}

/**
 * The function modifies the speed of the motor depending of the actual speed.
 *
 * @param [in] id Id of the dynamixel moduel
 * @param [in] more indicates if the speed must be added or substracted
 * @param [in] speed indicates the speed
 */
int changeSpeed(uint8_t id, bool more, uint16_t speed){
    if(!chk_id_motors(id)){return 1;}
    uint16_t actualSpeed;
    bool direction;
    int error = dyn_motor_read_speed(id,&actualSpeed,&direction);

    //La comprovació del máxim ja es fa quan s'assigna la velocitat.
    if(more){
        if(direction){
            actualSpeed = actualSpeed + speed;
        }else{
            if(speed<actualSpeed){
                actualSpeed = actualSpeed - speed;
            }else{
                actualSpeed = speed - actualSpeed;
                direction = 1;
            }
        }
    }else{
        if(direction){
            if(speed<actualSpeed){
                actualSpeed = actualSpeed - speed;
            }else{
                actualSpeed = speed - actualSpeed;
                direction = 0;
            }
        }else{
            actualSpeed = actualSpeed + speed;
        }
    }

    error += dyn_move(id,actualSpeed,direction);

    return error;
}


/**
 * Turn on in a continous mode the wheel with the speed given.
 *
 * @param [in] id Id of the dynamixel moduel
 * @param[in] speed Defines the velocity of the motors
 * @param[in] direction true means straight ahead, false means backwards
 */
int dyn_move(uint8_t id, uint16_t speed, bool direction) {
    if(!chk_id_motors(id)){return 1;}
    //Ens assegurem primer de tot de no sobrepassar la velocitat máxima.
    if(speed > VEL_MAX){
        speed = VEL_MAX;
    }
    //Ense assegurem de no sobrepassar la velocitat mínima
    if(speed < VEL_MIN){
        speed = VEL_MIN;
    }

    //Anem ara a construïr ara els dos registres que contenen la velocitat i direcció dels motors.
    uint8_t Mov_Speed_Low = speed % 256;
    uint8_t Mov_Speed_Hight = (speed / 256) + direction*2*2 ;
    uint8_t data[2] = {Mov_Speed_Low,Mov_Speed_Hight};
    dyn_write(id,DYN_REG__MOVL,data,2);
    dyn_continous(id);
}


/**
 * Turn the module in a contiunous mode.
 *
 * @param [in] id Id of the dynamixel moduel
 */
int dyn_continous(uint8_t id){
    if(!chk_id_motors(id)){return 1;}
    uint8_t data[4] = {0,0,0,0};
    dyn_write(id,DYN_REG__ANGLE,data,4);
}

/**
 * Read the current LED status
 *
 * @param[in] id Id of the dynamixel moduel
 * @param[out] val Current speed
 * @param[out] val2 Current direction
 */
int dyn_motor_read_speed(uint8_t id, uint16_t *val, bool *val2) {
    if(!chk_id_motors(id)){return 1;}
    uint8_t low;
    uint8_t high;

    int error = dyn_read_byte(id, DYN_REG__MOVH, &high) + dyn_read_byte(id, DYN_REG__MOVL, &low);
    *val = (low) + ((high)%4)*2*2*2*2*2*2*2*2;
    *val2 = high/4;
    return error;
}

/**
 * Checks if the id corresponds to one of the existing motors.
 *
 * @param[in] id Id of the dynamixel moduel
 */
bool chk_id_motors(uint8_t id){
    if(id==DYN_ID_MOTORR || id==DYN_ID_MOTORL){
        return true;
    }else{
        return false;
    }
}
