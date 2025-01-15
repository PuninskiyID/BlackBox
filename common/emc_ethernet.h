#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

#define P_UDP_MIN_DEBUG_M      20000
#define P_UDP_MAX_DEBUG_M      20010

#define P_UDP_MIN_ALIVE_M      30000
#define P_UDP_MAX_ALIVE_M      30010

#define P_UDP_MIN_CONFIG_M     40000
#define P_UDP_MAX_CONFIG_M     40010

#define P_UDP_MIN_CONTROL_M    50000
#define P_UDP_MAX_CONTROL_M    50010

#define P_UDP_MIN_DBG_NAMES_M  50100
#define P_UDP_MAX_DBG_NAMES_M  50110

#define P_UDP_DEBUG_L          20020
#define P_UDP_ALIVE_L          30020
#define P_UDP_CONFIG_L         40020
#define P_UDP_CONTROL_L        50020
#define P_UDP_DBG_NAMES_L      50120

#define SET_CNTRL	           0x01U
#define GET_CNTRL		       0x03U

typedef struct
{
	uint32_t valid;
	uint32_t unblock;
	uint32_t start_block_cause;
	uint32_t drive_en;
	uint32_t test_mode;
	uint32_t mode;
	double target_speed;
	double target_torque;
	double res0;
} t_emc_control;


#endif /* MAIN_H_ */
