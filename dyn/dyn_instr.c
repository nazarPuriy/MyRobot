/*
 * hal_dyn.c
 *
 *  Created on: 18 mar. 2020
 *      Author: droma
 *
 * This file implements the different dynamixel instructions
 */

#include "dyn/dyn_instr.h"
#include "dyn/dyn_frames.h"

/**
 * Single byte write instruction
 *
 * This function sends a write instruction for a single address position
 * to a given dynamixel module.
 *
 * @param[in] module_id Id of the dynamixel module
 * @param[in] reg_addr Address where the write is performed
 * @param[in] reg_write_val Value written to the previous address
 * @return Error code to be treated at higher levels.
 */
int dyn_write_byte(uint8_t module_id, DYN_REG_t reg_addr, uint8_t reg_write_val) {
    uint8_t parameters[2];
    struct RxReturn reply;

    parameters[0] = reg_addr;
    parameters[1] = reg_write_val;
    reply = RxTxPacket(module_id, 2, DYN_INSTR__WRITE, parameters);
    return (reply.tx_err << 1) | reply.time_out;
}

/**
 * Single byte read instruction
 *
 * This function sends a read instruction for a single address position
 * to a given dynamixel module.
 *
 * @param[in] module_id Id of the dynamixel module
 * @param[in] reg_addr Address where the read is performed
 * @param[out] reg_read_val Pointer where the read value is stored
 * @return Error code to be treated at higher levels.
 */
int dyn_read_byte(uint8_t module_id, DYN_REG_t reg_addr, uint8_t *reg_read_val) {
    uint8_t parameters[2];
    struct RxReturn reply;

    parameters[0] = reg_addr;
    parameters[1] = 1;
    reply = RxTxPacket(module_id, 2, DYN_INSTR__READ, parameters);
    *reg_read_val = reply.StatusPacket[5];

    return (reply.tx_err << 1) | reply.time_out;
}

/**
 * Multi-byte write instruction *
 * This function sends a write instruction starting at a given address position
 * with a given length for a dynamixel module.
 *
 * @param[in] module_id Id of the dynamixel module
 * @param[in] reg_addr Address where the write is performed
 * @param[in] val Pointer to the byte array to be written
 * @param[in] len Number of position to be written
 * @return Error code to be treated at higher levels.
 */
int dyn_write(uint8_t module_id, DYN_REG_t reg_addr, uint8_t *val, uint8_t len) {
    if(len>15){
        return 1; //Número de paràmetres no suportat.
    }
    uint8_t parameters[16];
    struct RxReturn reply;

    parameters[0] = reg_addr;
    int count = 0;
    for (count = 1; count < len + 1; count++) {
        parameters[count] = val[count-1];
    }

    reply = RxTxPacket(module_id, len + 1, DYN_INSTR__WRITE, parameters);


    return (reply.tx_err < 1) | reply.time_out;
}



