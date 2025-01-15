#ifndef _DCU_EMC_H_
#define _DCU_EMC_H_

#include <stdint.h>

#define CAN_ID(pri, orig, fcn, rcpt) ( ((pri)<<26) | ((fcn)<<16) | ((rcpt)<<8) | (orig) )
#define CAN_PF(can_id) ( (((can_id)>>16) & (0xFF)) )

#define CAN_SA_BCAST		0xFFUL
#define CAN_SA_DIAG			0x10UL
#define CAN_SA_DCU			0x11UL
#define CAN_SA_EMC1			0x12UL
#define CAN_SA_EMC2			0x13UL
#define CAN_SA_EMC3			0x14UL
#define CAN_SA_ACS			0x15UL
#define CAN_SA_UCC			0x16UL
#define CAN_SA_IC			0x17UL
#define CAN_SA_CP			0x18UL
#define CAN_SA_BCU			0x19UL
#define CAN_SA_HVAC			0x1AUL
#define CAN_SA_LCM_ALL		0x20UL
#define CAN_SA_LCM_LRE		0x21UL
#define CAN_SA_LCM_LFR		0x22UL
#define CAN_SA_LCM_RRE		0x23UL
#define CAN_SA_LCM_RFR		0x24UL
#define CAN_SA_L_EPW		0x40UL
#define CAN_SA_R_EPW		0x41UL
#define CAN_SA_F_WCM		0x50UL
#define CAN_SA_R_WCM		0x51UL
#define CAN_SA_HVAC_CP		0x60UL
#define CAN_SA_BUTTONPNL	0x61UL

#define CAN_PF_EMC_CON		0xFFUL




/*
 * About precision & offset
 * If precision not specified - assume "1.0"
 * If offset not specified - assume "0.0"
 *
 * At transmitting side: can_value = (real_value - offset) / precision;
 * At receiving side: real_value = can_value * precision + offset;
 */


/*
 * UCC output message
 * Direction: UCC -> DCU
 */

#define CAN_PF_UCC_OUT_CYCLE_MS		100UL
#define CAN_PF_UCC_OUT				0x01UL
typedef struct
{
#ifdef C2000
	int16_t high_voltage;					// "V", "precision 1"
	int16_t supercap_voltage;				// "V", "precision 1"
	int16_t leakage_resistance;				// "kOhm", "precision 1"
	struct
	{
		uint16_t error:8;
		uint16_t status:8;
	} bits;
#else
	int16_t high_voltage;					// "V", "precision 1"
	int16_t supercap_voltage;				// "V", "precision 1"
	int16_t leakage_resistance;				// "kOhm", "precision 1"
	uint8_t error;
	uint8_t status;
#endif


#define CAN_PF_UCC_ERROR_CHARGING						0x01UL
#define CAN_PF_UCC_ERROR_LEAKAGE_RESIST_WRN				0x02UL
#define CAN_PF_UCC_ERROR_LEAKAGE_RESIST					0x04UL
#define CAN_PF_UCC_ERROR_CONTACTOR						0x08UL
#define CAN_PF_UCC_ERROR_LOW_INPUT_VOLTAGE				0x10UL

#define CAN_PF_UCC_STATUS_OFF							0x00UL
#define CAN_PF_UCC_STATUS_PRECHARGE						0x01UL
#define CAN_PF_UCC_STATUS_ON							0x02UL

}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ucc_out;
/*
 * UCC control message
 * Direction: DCU -> UCC
 */

#define CAN_PF_UCC_CNTRL_CYCLE_MS		100UL
#define CAN_PF_UCC_CNTRL	0x01UL
typedef struct
{
	int16_t on;									// "flag"
	int16_t service_on;							// "flag"
	int16_t r1;
	int16_t r2;
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ucc_cntrl;

/*
 * ACS(Autonomous current sensor) output message
 * Direction: ACS -> all
 */

#define CAN_PF_ACS_OUT_CYCLE_MS		10UL
#define CAN_PF_ACS_OUT				0x01UL
typedef struct
{
	int16_t current0;							// "A", "precision 1"
    int16_t current1;							// "A", "precision 1"
    int16_t current2;							// "A", "precision 1"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_acs_out;

/*
 * DCU request to change drive mode
 * Direction: IC -> DCU
 */

#define CAN_PF_DCU_PRESET_CYCLE_MS		1000UL
#define CAN_PF_DCU_PRESET				0x05UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t	preset		:	8;
	}bits;
#else
		uint8_t preset;							// "enumeration"
#endif

#define CAN_PF_DCU_PRESET_ECO		0x00
#define CAN_PF_DCU_PRESET_CITY		0x01
#define CAN_PF_DCU_PRESET_HIGHWAY	0x02
#define CAN_PF_DCU_PRESET_WINTER	0x03
#define CAN_PF_DCU_PRESET_SPORT		0x04
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_preset;

/*
 * DCU trip statistics output message 0
 * Direction: DCU -> IC
 */

#define CAN_PF_DCU_STAT_TRIP0_CYCLE_MS		1000UL
#define CAN_PF_DCU_STAT_TRIP0	0x06UL
typedef struct
{
	uint16_t consumption;						// "l/100km", "precision 0.1"
	uint16_t max_spd;							// "km/h", "precision 0.1"
	uint32_t total_time;						// "s", "precision 1"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_stat_trip0;

/*
 * DCU trip statistics output message 1
 * Direction: DCU -> IC
 */

#define CAN_PF_DCU_STAT_TRIP1_CYCLE_MS		1000UL
#define CAN_PF_DCU_STAT_TRIP1	0x07UL
typedef struct
{
	uint32_t move_time;							// "s", "precision 1"
	uint32_t distance;							// "m", "precision 1"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_stat_trip1;

/*
 * DCU last driving session statistics output message 0
 * Direction: DCU -> IC
 */

#define CAN_PF_DCU_STAT_SESSION0_CYCLE_MS		1000UL
#define CAN_PF_DCU_STAT_SESSION0	0x08UL
typedef struct
{
	uint16_t consumption;						// "l/100km", "precision 0.1"
	uint16_t max_spd;							// "km/h", "precision 0.1"
	uint32_t total_time;						// "s", "precision 1"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_stat_session0;

/*
 * DCU last driving session statistics output message 1
 * Direction: DCU -> IC
 */

#define CAN_PF_DCU_STAT_SESSION1_CYCLE_MS		1000UL
#define CAN_PF_DCU_STAT_SESSION1	0x09UL
typedef struct
{
	uint32_t move_time;							// "s", "precision 1"
	uint32_t distance;							// "m", "precision 1"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_stat_session1;

/*
 * DCU request to reset statistics data
 * Direction: IC -> DCU
 */

#define CAN_PF_DCU_STAT_RESET_CYCLE_MS		0UL
#define CAN_PF_DCU_STAT_RESET	0x0AUL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t type : 8;
	} bits;
#else
	uint8_t type;									// "enumeration"
#endif
#define CAN_PF_DCU_STAT_RESET_TYPE_CONS		0x01
#define CAN_PF_DCU_STAT_RESET_TYPE_TRIP		0x02
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_stat_reset;

/*
 * DCU average speed statistics
 * Direction: DCU -> IC
 */

#define CAN_PF_DCU_STAT_AVG_SPEED_CYCLE_MS		1000UL
#define CAN_PF_DCU_STAT_AVG_SPEED				0x0BUL
typedef struct
{
	uint16_t avg_speed_ses;						// "km/h", "precision 0.1"
	uint16_t avg_speed_move_ses;				// "km/h", "precision 0.1"
	uint16_t avg_speed_trip;					// "km/h", "precision 0.1"
	uint16_t avg_speed_move_trip;				// "km/h", "precision 0.1"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_stat_avg_speed;

/*
 * DCU request to change driver display mode
 * Direction: IC -> DCU
 */

#define CAN_PF_DCU_SET_DDM_MODE_CYCLE_MS		0UL
#define CAN_PF_DCU_SET_DDM_MODE					0x0CUL
typedef struct
{
	uint8_t mode;						// "enum"
#define CAN_PF_DCU_SET_DDM_MODE_NEXT			0x00
#define CAN_PF_DCU_SET_DDM_MODE_SPEED			0x01
#define CAN_PF_DCU_SET_DDM_MODE_SPEED_AVG		0x02
#define CAN_PF_DCU_SET_DDM_MODE_SPEED_MAX		0x03
#define CAN_PF_DCU_SET_DDM_MODE_SPEED_CRUISE	0x04
#define CAN_PF_DCU_SET_DDM_MODE_TRIP_TIME		0x05
#define CAN_PF_DCU_SET_DDM_MODE_TRIP_TIME_MOVE	0x06
#define CAN_PF_DCU_SET_DDM_MODE_CONSUMPTION		0x07
#define CAN_PF_DCU_SET_DDM_MODE_RANGE			0x08
#define CAN_PF_DCU_SET_DDM_MODE_TRIP_DISTANCE	0x09
#define CAN_PF_DCU_SET_DDM_MODE_VBAT			0x0A
#define CAN_PF_DCU_SET_DDM_MODE_UCAP			0x0B
#define CAN_PF_DCU_SET_DDM_MODE_DBG_TEMP		0x0C
#define CAN_PF_DCU_SET_DDM_MODE_DBG_ERR			0x0D
#define CAN_PF_DCU_SET_DDM_MODE_DBG_RPM			0x0E
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_ddm_mode;


/*
 * DCU status message 0
 * Direction: DCU -> all
 */

#define 	CAN_PF_DCU_STATUS0_CYCLE_MS							20UL
#define		CAN_PF_DCU_STATUS0									0x20UL
typedef struct								// Broadcast
{
#ifdef C2000
	struct
	{
		int16_t vehicle_speed;
		uint16_t cruise_speed :8;
		uint16_t gear :8;
		uint16_t acc_pos :8;
		uint16_t brk_pos :8;
		int16_t uc_current_norm :8;
	} bits;
#else
	int16_t vehicle_speed;					// "km/h", "precision 0.01", "range -327.6 .. 327.6"
	uint8_t cruise_speed;					// "km/h", "precision 1.0", "range 0 .. 255"
	uint8_t gear;							// "bit field"
	uint8_t acc_pos;						// "%", "precision 0.392", "range 0 .. 99.96"
	uint8_t brk_pos;						// "%", "precision 0.392", "range 0 .. 99.96"
	int8_t uc_current_norm;					// "%", "precision 1.0", "range -100 .. 100"
#endif

#define CAN_PF_DCU_STATUS0_GEAR_PARK				0x00UL
#define CAN_PF_DCU_STATUS0_GEAR_N					0x01UL
#define CAN_PF_DCU_STATUS0_GEAR_D					0x02UL
#define CAN_PF_DCU_STATUS0_GEAR_R					0x03UL

}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_status0;


/*
 * DCU status message 1
 * Direction: DCU -> all
 */

#define CAN_PF_DCU_STATUS1_CYCLE_MS							1000UL
#define	CAN_PF_DCU_STATUS1									0x21UL
typedef struct							// Broadcast
{
#ifdef C2000
	struct
	{
		uint16_t range;
		uint16_t fuel_level :8;
		uint16_t range_scale :8;
		uint16_t charge :8;
		uint16_t temp_el_norm :8;
		uint16_t temp_ice_norm :8;
		uint16_t uc_voltage :8;
	} bits;
#else
	uint16_t range;							// "km", "precision 1.0", "range 0 .. 65535"
	uint8_t fuel_level;						// "%", "precision 0.392", "range 0 .. 99.96"
	uint8_t range_scale;					// "km", "precision 10.0", "range 0 .. 2550"
	uint8_t charge;							// "%", "precision 0.392", "range 0 .. 99.96"
	uint8_t temp_el_norm;					// "%", "precision 0.392", "range 0 .. 99.96"
	uint8_t temp_ice_norm;					// "%", "precision 0.392", "range 0 .. 99.96"
	uint8_t uc_voltage;						// "%", "precision 2.0", "range 0 .. 512"
#endif
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_status1;


/*
 * DCU status message 2
 * Direction: DCU -> all
 */

#define CAN_PF_DCU_STATUS2_CYCLE_MS							1000UL
#define	CAN_PF_DCU_STATUS2									0x22UL
typedef struct
{
	uint32_t total_distance;			// "km", "precision 0.1", "range 0 .. 429496729.5"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_status2;

/*
 * DCU status message 3
 * Direction: DCU -> all
 */

#define CAN_PF_DCU_STATUS3_CYCLE_MS							100UL
#define CAN_PF_DCU_STATUS3									0x23UL
typedef struct
{
	uint16_t gen_current;			// "A", "precision 0.1", "range -3276.7 .. 3276.7", "offset -3276.7"
	uint16_t cap_current;			// "A", "precision 0.1", "range -3276.7 .. 3276.7", "offset -3276.7"
	uint16_t voltage;				// "V", "precision 0.1", "range -3276.7 .. 3276.7", "offset -3276.7"
	uint8_t axis_balance;			// "%", "precision 1", "range 0 .. 100", "100 - all rear, 0 - all front"
	uint8_t rext;				// "enum"

#define CAN_PF_DCU_STATUS3_REXT_OFF				0x00UL
#define CAN_PF_DCU_STATUS3_REXT_ON				0x01UL
#define CAN_PF_DCU_STATUS3_REXT_ON_IDLE			0x02UL

}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_dcu_status3;


/*
 * IC error signs
 * Direction: DCU -> IC
 */

#define CAN_PF_IC_ERROR_SIGNS_CYCLE_MS						1000UL
#define CAN_PF_IC_ERROR_SIGNS								0x30UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t s1 :8;
		uint16_t s2 :8;
		uint16_t s3 :8;
		uint16_t s4 :8;
		uint16_t s5 :8;
		uint16_t s6 :8;
		uint16_t s7 :8;
		uint16_t s8 :8;
	} bits;
#endif
	uint8_t s1;
	uint8_t s2;
	uint8_t s3;
	uint8_t s4;
	uint8_t s5;
	uint8_t s6;
	uint8_t s7;
	uint8_t s8;
#define CAN_PF_IC_ERROR_SIGNS_S1_PARK_BRAKE			(2 << 0)
#define CAN_PF_IC_ERROR_SIGNS_S1_OIL				(2 << 2)
#define CAN_PF_IC_ERROR_SIGNS_S1_GEN_FAULT			(2 << 4)
#define CAN_PF_IC_ERROR_SIGNS_S1_BRAKE_FAULT		(2 << 6)
#define CAN_PF_IC_ERROR_SIGNS_S2_ABS_FAULT			(2 << 0)
#define CAN_PF_IC_ERROR_SIGNS_S2_ESP_FAULT			(2 << 2)
#define CAN_PF_IC_ERROR_SIGNS_S2_ICE_FAULT			(2 << 4)
#define CAN_PF_IC_ERROR_SIGNS_S2_EMOTOR_FAULT		(2 << 6)
#define CAN_PF_IC_ERROR_SIGNS_S3_RESERV0			(2 << 0)
#define CAN_PF_IC_ERROR_SIGNS_S3_RESRVE1			(2 << 2)
#define CAN_PF_IC_ERROR_SIGNS_S3_ICE_OVERTEMP		(2 << 4)
#define CAN_PF_IC_ERROR_SIGNS_S3_EMOTOR_OVERTEMP	(2 << 6)
#define CAN_PF_IC_ERROR_SIGNS_S4_CRUISE				(2 << 0)
#define CAN_PF_IC_ERROR_SIGNS_S4_SEAT_BELT			(2 << 2)
#define CAN_PF_IC_ERROR_SIGNS_S4_RESRVE0			(2 << 4)
#define CAN_PF_IC_ERROR_SIGNS_S4_RESERVE1			(2 << 6)
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_error_signs;


/*
 * IC buzzer target
 * Direction: any -> IC
 */

#define CAN_PF_IC_BUZZER_CYCLE_MS							0UL
#define CAN_PF_IC_BUZZER									0x31UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t freq;
		uint16_t time_ms;
		uint16_t intens :8;
	} bits;
#endif
	uint16_t freq;					// "Hz", "precision 1.0", "range 10 .. 20000"
	uint16_t time_ms;				// "km", "precision 1.0", "range 1 .. 2000"
	uint8_t intens;					// "%", "precision 1.0", "range 0 .. 100"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_buzzer;

/*
 * IC debug info 0
 * Direction: any -> IC
 */

#define CAN_PF_IC_DEBUG0_CYCLE_MS							0UL
#define	CAN_PF_IC_DEBUG0									0x32UL
typedef struct
{
	uint32_t hex1;
	uint32_t hex2;
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_debug0;


/*
 * IC debug info 1
 * Direction: any -> IC
 */

#define CAN_PF_IC_DEBUG1_CYCLE_MS							0UL
#define	CAN_PF_IC_DEBUG1									0x33UL
typedef struct
{
	uint32_t hex1;
	uint32_t hex2;
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_debug1;

/*
 * IC debug info 2
 * Direction: any -> IC
 */

#define CAN_PF_IC_DEBUG2_CYCLE_MS							0UL
#define	CAN_PF_IC_DEBUG2									0x34UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t dec1 :8;
		uint16_t dec2 :8;
		uint16_t dec3 :8;
		uint16_t dec4 :8;
		uint16_t dec5 :8;
		uint16_t dec6 :8;
		uint16_t dec7 :8;
		uint16_t dec8 :8;
	} bits;
#endif
	uint8_t dec1;
	uint8_t dec2;
	uint8_t dec3;
	uint8_t dec4;
	uint8_t dec5;
	uint8_t dec6;
	uint8_t dec7;
	uint8_t dec8;
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_debug2;


/*
 * CP buttons press/long-press/release events
 * Direction: CP -> IC
 */

#define CAN_PF_CP_BUTTON_CYCLE_MS			0x00UL
#define CAN_PF_CP_BUTTON                	0x01UL

typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t id   : 8;
		uint16_t type : 8;
	} bits;
#else
	uint8_t id;										// "enumeration"
	uint8_t type;									// "enumeration"
#endif
#define CAN_PF_CP_BUTTON_ID_BOARD_COMP		0x05UL
#define CAN_PF_CP_BUTTON_ID_CLIMAT			0x04UL
#define CAN_PF_CP_BUTTON_ID_NAVIGATION		0x03UL
#define CAN_PF_CP_BUTTON_ID_MEDIA			0x00UL
#define CAN_PF_CP_BUTTON_ID_PHONE			0x01UL
#define CAN_PF_CP_BUTTON_ID_SETTINGS		0x02UL
#define CAN_PF_CP_BUTTON_ID_BACK			0x07UL
#define CAN_PF_CP_BUTTON_ID_MENU			0x09UL
#define CAN_PF_CP_BUTTON_ID_HOME			0x06UL
#define CAN_PF_CP_BUTTON_ID_APPLICATION		0x08UL
#define CAN_PF_CP_BUTTON_ID_ENTER			0x0AUL

#define CAN_PF_CP_BUTTON_TYPE_PRESS			0x01UL
#define CAN_PF_CP_BUTTON_TYPE_RELEASE		0x02UL
#define CAN_PF_CP_BUTTON_TYPE_LONG_PRESS	0x03UL
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_cp_button;


/*
 * CP encoder events
 * Direction: CP -> IC
 */

#define CAN_PF_CP_ENCODER_CYCLE_MS				0x00UL
#define CAN_PF_CP_ENCODER                       0x02UL


typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t shift   		:	8;
		uint16_t direction 		:	8;
	} bits;
#else
	uint8_t shift;		    						// "1", "precision 1"
	uint8_t direction;	    						// "enumeration"
#endif
#define CAN_PF_CP_ENCODER_DIRECTION_POS		0x00UL
#define CAN_PF_CP_ENCODER_DIRECTION_NEG		0x01UL
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_cp_encoder;


/*
 * CP touch panel events
 * Direction: CP -> IC
 */

#define CAN_PF_CP_TOUCH_CYCLE_MS           		0x00UL
#define CAN_PF_CP_TOUCH                 		0x03UL

typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t xh_1		:8;
		uint16_t xl_1		:8;
		uint16_t yh_1		:8;
		uint16_t yl_1		:8;
		uint16_t xh_2		:8;
		uint16_t xl_2		:8;
		uint16_t yh_2		:8;
		uint16_t yl_2		:8;
	} bits;
#else
	uint8_t xh_1;
	uint8_t xl_1;
	uint8_t yh_1;
	uint8_t yl_1;
	uint8_t xh_2;
	uint8_t xl_2;
	uint8_t yh_2;
	uint8_t yl_2;

#endif
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_cp_touch;

/*
 * CP request to change buttons backlight color/intensity
 * Direction: IC -> CP
 */

#define CAN_PF_CP_LEDS_CYCLE_MS						0UL
#define CAN_PF_CP_LEDS								0xB2UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t red   		:	8;
		uint16_t green 		:	8;
		uint16_t blue	 	:	8;
	} bits;
#else
	uint8_t red;	    							// "%", "precision 100/256"
	uint8_t green;									// "%", "precision 100/256"
	uint8_t blue;	    							// "%", "precision 100/256"
#endif
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_cp_leds;

/*
 * CP sends brightness to BCU
 * Direction: CP -> BCU
 */

#define CAN_PF_CP_BRIGHTNESS_CYCLE_MS		0UL
#define CAN_PF_CP_BRIGHTNESS		0xB6UL
typedef struct
{
	uint16_t light_sensor_value;
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_cp_brightness;


/*
 * BCU sends brightness to all devices
 * Direction: BCU -> ALL
 */

#define CAN_PF_BCU_BRIGHTNESS_TARGET_CYCLE_MS		0UL
#define CAN_PF_BCU_BRIGHTNESS_TARGET							0xB0UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t brightness_upper_lcd : 8;
		uint16_t brightness_lower_lcd : 8;
		uint16_t brightness_driver_oled : 8;
		uint16_t brightness_leds : 8;
		uint16_t brightness_speed;
	} bits;
#else
	uint8_t brightness_upper_lcd;	    							// 1 "%", "0 ... 100"
	uint8_t brightness_lower_lcd;	    							// 1 "%", "0 ... 100"		!
	uint8_t brightness_driver_oled;								// 1 "%", "0 ... 100"
	uint8_t brightness_leds;	    									// 1 "%", "0 ... 100"
	uint16_t brightness_speed;										// (0.1 "%") / s
#endif
}

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_bcu_brightness_target;

/*
 * BCU brightness control
 * Direction: IC -> BCU
 */

#define CAN_PF_IC_BRIGHTNESS_CNTRL_CYCLE_MS		0UL
#define CAN_PF_IC_BRIGHTNESS_CNTRL							0xB4UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t bits : 8;
	} bits;
#else
	uint8_t bits;
#endif
}
#define CAN_IC_BRIGHTENSS_CNTRL_BITS_AUTO									0x0001UL
#define CAN_IC_BRIGHTENSS_CNTRL_BITS_TEMP_DAY							0x0002UL
#define CAN_IC_BRIGHTENSS_CNTRL_BITS_TEMP_NIGHT						0x0004UL
#define CAN_IC_BRIGHTENSS_CNTRL_BITS_DAY_UP									0x0008UL
#define CAN_IC_BRIGHTENSS_CNTRL_BITS_DAY_DW								0x0010UL
#define CAN_IC_BRIGHTENSS_CNTRL_BITS_NIGHT_UP							0x0020UL
#define CAN_IC_BRIGHTENSS_CNTRL_BITS_NIGHT_DW							0x0040UL
#define CAN_IC_BRIGHTENSS_CNTRL_BITS_SAVE										0x0080UL

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_brightness_cntrl;

/*
 * BCU send brightness status
 * Direction: BCU -> IC
 */

#define CAN_PF_BCU_BRIGHTNESS_STATUS_CYCLE_MS		0UL
#define CAN_PF_BCU_BRIGHTNESS_STATUS							0xB5UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t mode_auto : 8;
		uint16_t brightness_day : 8;						// 1 "%", "0 ... 100"
		uint16_t brightness_night : 8;					// 1 "%", "0 ... 100"
	} bits;
#else
	uint8_t mode_auto;
	uint8_t brightness_day;
	uint8_t brightness_night;
#endif
}
#define CAN_BCU_BRIGHTENSS_MODE_AUTO_ON							0x0001UL
#define CAN_BCU_BRIGHTENSS_MODE_AUTO_OFF						0x0000UL

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_bcu_brightness_status;

/*
 * BCU send targets on door locks
 * Direction: BCU -> ALL
 */

#define CAN_PF_BCU_DOOR_LOCKS_CYCLE_MS						100UL
#define CAN_PF_BCU_DOOR_LOCKS_TARG								0xB8UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t close : 8;
		uint16_t res_0 : 8;
	} bits;
#else
	uint16_t lock;
	uint16_t res_0;
#endif
}
#define CAN_PF_BCU_DOOR_LOCKS_CLOSE							0x0001UL
#define CAN_PF_BCU_DOOR_LOCKS_OPEN								0x0000UL

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_bcu_door_locks_targ;

/*
 * DCU send to BCU target on switch on/off PSU
 * Direction: DCU -> BCU
 */

#define CAN_PF_BCU_HVPSU_CONTROL_CYCLE_MS		100UL
#define CAN_PF_BCU_HVPSU_CONTROL							0xB6UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t enable : 8;
	} bits;
#else
	uint8_t enable;
#endif
}
#define CAN_BCU_HVPSU_CONTROL_ENABLE_ON					0x0001UL
#define CAN_BCU_HVPSU_CONTROL_ENABLE_OFF					0x0000UL

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_bcu_hvpsu_control;

////////////////////////////////////////////////////////////////
/*
 * BCU send to DCU HVPSU pump request
 * Direction: BCU -> DCU
 */

#define CAN_PF_BCU_HVPSU_PUMP_REQ_CYCLE_MS		100UL
#define CAN_PF_BCU_HVPSU_PUMP_REQ				0xB7UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t enable : 8;
	} bits;
#else
	uint8_t enable;
#endif
}
#define CAN_BCU_HVPSU_PUMP_REQ_ENABLE_ON					0x0001UL
#define CAN_BCU_HVPSU_PUMP_REQ_ENABLE_OFF					0x0000UL

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_bcu_hvpsu_pump_req;

////////////////////////////////////////////////////////////////

/*
 * CP request to change UP LCD source
 * Direction: CP -> IC
 */

#define CAN_PF_CP_UP_LCD_SOURCE_CONTROL_CYCLE_MS		0UL
#define CAN_PF_CP_UP_LCD_SOURCE_CONTROL                         0xB1UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t up_lcd_source_control : 8;
	} bits;
#else
	uint8_t up_lcd_source_control;
#endif
}
//    bit0 -    0- LPC1788  signal source
//              1- MX53
//    bit1 -    0- immediate switch
//              1- smooth switch
#define CAN_CP_SOURCE_CONTROL_LPC1788												0x0000UL
#define CAN_CP_SOURCE_CONTROL_MX53                             					0x0001UL
#define CAN_CP_SOURCE_CONTROL_SWITCH_IMMED                            0x0000UL
#define CAN_CP_SOURCE_CONTROL_SWITCH_SMOOTH							0x0002UL

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_cp_source_control;

/*
 * EMC control message
 * Direction: DCU -> EMC
 */

#define CAN_PF_EMC_CNTRL_CYCLE_MS		5UL
#define CAN_PF_EMC_CNTRL	0x01UL
typedef struct
{
	int16_t torque;									// "Nm", "precision 0.01", "range -327.0 .. 327.0"
	int16_t speed;									// "rad/s", "precision 1", "range -32700 .. 32700"
	uint16_t r0;
#ifdef C2000
	struct
	{
		uint16_t speed_control : 8;					// "flag"
		uint16_t enable : 8;						// "enum"
	} bits;
#else
	uint8_t mode;									// "flag"
	uint8_t enable;           						// "enum"
#endif

#define CAN_PF_EMC_CNTRL_MODE_TORQUE				0
#define CAN_PF_EMC_CNTRL_MODE_SPEED_MOT_GEN			1
#define CAN_PF_EMC_CNTRL_MODE_SPEED_GEN_ONLY		2
#define CAN_PF_EMC_CNTRL_MODE_SPEED_MOT_ONLY		3

#define CAN_PF_EMC_CNTRL_ENABLE_REQ_DISABLE			0
#define CAN_PF_EMC_CNTRL_ENABLE_REQ_UNBLOCK			1
#define CAN_PF_EMC_CNTRL_ENABLE_REQ_ENABLE			2
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_emc_cntrl;

/*
 * EMC output message
 * Direction: EMC -> DCU
 */


#define CAN_PF_EMC_OUT_CYCLE_MS		5UL
#define CAN_PF_EMC_OUT		0x02UL
typedef struct
{
	uint16_t sb; 							// "bit field"
#define CAN_PF_EMC_OUT_SB_INITIAL		(1U << 15) //8000
#define CAN_PF_EMC_OUT_SB_INITIAL_TXT		"Blocked due to power-on"
#define CAN_PF_EMC_OUT_SB_CAN			(1U << 14) //4000
#define CAN_PF_EMC_OUT_SB_CAN_TXT			"CAN fault"
#define CAN_PF_EMC_OUT_SB_INV_OT		(1U << 13) //2000
#define CAN_PF_EMC_OUT_SB_INV_OT_TXT		"Inverter over temperature fault"
#define CAN_PF_EMC_OUT_SB_MOTOR_OT		(1U << 12) //1000
#define CAN_PF_EMC_OUT_SB_MOTOR_OT_TXT		"Motor over temperature fault"
#define CAN_PF_EMC_OUT_SB_CURRENT		(1U << 11) //0800
#define CAN_PF_EMC_OUT_SB_CURRENT_TXT		"Current limit fault"
#define CAN_PF_EMC_OUT_SB_SPEED			(1U << 10) //0400
#define CAN_PF_EMC_OUT_SB_SPEED_TXT			"Maximum speed fault"
#define CAN_PF_EMC_OUT_SB_RESOLVER		(1U << 9) //0200
#define CAN_PF_EMC_OUT_SB_RESOLVER_TXT		"Resolver fault"
#define CAN_PF_EMC_OUT_SB_NVMEM			(1U << 8) //0100
#define CAN_PF_EMC_OUT_SB_NVMEM_TXT			"Non-volatile memory fault"
#define CAN_PF_EMC_OUT_SB_HV			(1U << 7) //0080
#define CAN_PF_EMC_OUT_SB_HV_TXT			"High voltage fault"
#define CAN_PF_EMC_OUT_SB_VBAT			(1U << 6) //0040
#define CAN_PF_EMC_OUT_SB_VBAT_TXT			"Bus voltage fault"
#define CAN_PF_EMC_OUT_SB_ESM			(1U << 5) //0020
#define CAN_PF_EMC_OUT_SB_ESM_TXT			"Error signaling module fault"
#define CAN_PF_EMC_OUT_SB_ETH			(1U << 4) //0010
#define CAN_PF_EMC_OUT_SB_ETH_TXT			"Ethernet fault"
#define CAN_PF_EMC_OUT_SB_TEMP_SENSOR	(1U << 3) //0008
#define CAN_PF_EMC_OUT_SB_TEMP_SENSOR_TXT	"Temperature sensor fault"
#define CAN_PF_EMC_OUT_SB_DRIVER		(1U << 2) //0004
#define CAN_PF_EMC_OUT_SB_DRIVER_TXT		"IGBT driver fault"
#define CAN_PF_EMC_OUT_SB_INSANE_CNTRL		(1U << 1) //0002
#define CAN_PF_EMC_OUT_SB_INSANE_CNTRL_TXT	"Insane control"
	int16_t actual_torque;					// "Nm", "precision 0.01", "range -655.35 .. 655.35"
	int16_t voltage;						// "V", "precision 1", "range 0 .. 2000"
	int16_t wr;								// "rad/s", "precision 1", "range -3000 .. 3000"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_emc_out;

/*
 * EMC output message 1
 * Direction: EMC -> DCU
 */

#define CAN_PF_EMC_OUT1_CYCLE_MS		100UL
#define CAN_PF_EMC_OUT1		0x03UL
typedef struct
{
	int16_t ti0;									// "degC", "precision 1"
	int16_t ti1;									// "degC", "precision 1"
	int16_t ti2;									// "degC", "precision 1"
	int16_t tm;										// "degC", "precision 1"
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_emc_out1;

/*
 * EMC output message 2
 * Direction: EMC -> DCU
 */

#define CAN_PF_EMC_OUT2_CYCLE_MS		100UL
#define CAN_PF_EMC_OUT2		0x04UL
typedef struct
{
	uint64_t turns;									// "1", "precision 1"
#if __SIZEOF_INT__ == 8
} __attribute__((__packed__)) t_can_emc_out2;
#else
} t_can_emc_out2;
#endif

/*
 * BCU steering wheel buttons press/long-press/release events
 * Direction: BCU -> IC
 */
#define CAN_PF_BCU_SWHEEL_BUTTONS_CYCLE_MS	0UL
#define CAN_PF_BCU_SWHEEL_BUTTONS               0xB3UL

typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t id   : 8;
		uint16_t type : 8;
	} bits;
#else
	uint8_t id;										// "enumeration"
	uint8_t type;									// "enumeration"
#endif
//id
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_PHONE					0x01UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_VOL_UP					0x02UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_VOL_DW					0x03UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_PREV						0x04UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_NEXT						0x05UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_PAUSE						0x06UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_LEFT						0x07UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_RIGHT						0x08UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_UP							0x09UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_DOWN						0x0AUL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_OK							0x0BUL
#define CAN_PF_BCU_SWHEEL_BUTTONS_ID_MODE						0x0CUL
//type
#define CAN_PF_BCU_SWHEEL_BUTTONS_TYPE_PRESS					0x01UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_TYPE_RELEASE			0x02UL
#define CAN_PF_BCU_SWHEEL_BUTTONS_TYPE_LONG_PRESS		0x03UL
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_bcu_swheel_buttons;

/*
 * HVAC control message
 * Direction: IC -> HVAC
 */

#define CAN_PF_HVAC_CONTROL_CYCLE_MS		0UL
#define	CAN_PF_HVAC_CONTROL									0x01UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t id   : 8;
	} bits;
#else
	uint8_t id;									// "enumeration"
#endif

#define CAN_PF_HVAC_CONTROL_ID_FAN_UP						0x00UL
#define CAN_PF_HVAC_CONTROL_ID_FAN_DWN						0x01UL
#define CAN_PF_HVAC_CONTROL_ID_TEMP_UP						0x02UL
#define CAN_PF_HVAC_CONTROL_ID_TEMP_DWN						0x03UL
#define CAN_PF_HVAC_CONTROL_ID_COOLING						0x04UL
#define CAN_PF_HVAC_CONTROL_ID_FR_DEFROST					0x05UL
#define CAN_PF_HVAC_CONTROL_ID_RE_DEFROST					0x06UL
#define CAN_PF_HVAC_CONTROL_ID_RECYRC						0x07UL
#define CAN_PF_HVAC_CONTROL_ID_BLOW_FOOT					0x08UL
#define CAN_PF_HVAC_CONTROL_ID_BLOW_HEAD					0x09UL
#define CAN_PF_HVAC_CONTROL_ID_BLOW_WINDOW					0x0AUL
#define CAN_PF_HVAC_CONTROL_ID_AUTO						0x0BUL
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_hvac_control;

/*
 * HVAC output message
 * Direction: HVAC -> IC
 */

#define CAN_PF_HVAC_STATUS_CYCLE_MS		1000UL
#define	CAN_PF_HVAC_STATUS									0x02UL
typedef struct
{
#ifdef C2000
	struct
	{
		int16_t ext_temp  		:8;
		int16_t int_temp_set	:8;
		uint16_t fan_speed		:8;
		uint16_t bits		:8;
	} bits;
#else
	int8_t ext_temp;							// "degC", "precision 1"
	int8_t int_temp_set;						// "degC", "precision 1"
	uint8_t fan_speed;							// "1", "precision 1"
	uint8_t bits;								// "bit field"
#endif

#define CAN_PF_HVAC_STATUS_BITS_COOLING						0x01UL
#define CAN_PF_HVAC_STATUS_BITS_FR_DEFROST					0x02UL
#define CAN_PF_HVAC_STATUS_BITS_RE_DEFROST					0x04UL
#define CAN_PF_HVAC_STATUS_BITS_RECYRC						0x08UL
#define CAN_PF_HVAC_STATUS_BITS_BLOW_FOOT					0x10UL
#define CAN_PF_HVAC_STATUS_BITS_BLOW_HEAD					0x20UL
#define CAN_PF_HVAC_STATUS_BITS_BLOW_WINDOW					0x40UL
#define CAN_PF_HVAC_STATUS_BITS_AUTO						0x80UL
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_hvac_status;



/*
 * HVAC output control message
 * Direction: HVAC -> ALL
 */

#define CAN_PF_HVAC_CONTROL_DEFROST_CYCLE_MS			1000UL
#define	 CAN_PF_HVAC_CONTROL_DEFROST						0x03UL
typedef struct
{
		uint16_t defrost_control;
#define CAN_PF_HVAC_DEFROST_LEFT_MIRROR						0x01UL
#define CAN_PF_HVAC_DEFROST_RIGHT_MIRROR					0x02UL
#define CAN_PF_HVAC_DEFROST_REAR_WINDOW 					0x04UL

}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_hvac_defrost_control;

/*
 * HVAC fan & pump request message
 * Direction: HVAC -> DCU
 */

#define CAN_PF_HVAC_FAN_PUMP_CYCLE_MS					100UL
#define	 CAN_PF_HVAC_FAN_PUMP							0x04UL
typedef struct
{
		uint8_t req;						// "bit field"
#define CAN_PF_HVAC_FAN_PUMP_REQ_FAN1						0x01UL
#define CAN_PF_HVAC_FAN_PUMP_REQ_FAN2						0x02UL
#define CAN_PF_HVAC_FAN_PUMP_REQ_PUMP						0x04UL
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_hvac_fan_pump;


/*
 * IC current time/date & request to set time/date
 * Direction: IC -> all
 * Direction: any -> IC
 */

#define CAN_PF_IC_DATE_TIME_CYCLE_MS						1000UL
#define CAN_PF_IC_DATE_TIME_STATUS							0xA0UL
#define CAN_PF_IC_DATE_TIME_SET								0xA1UL
typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t h  		:8;
		uint16_t m			:8;
		uint16_t s			:8;
		uint16_t day		:8;
		uint16_t month		:8;
		uint16_t year_h		:8;
		uint16_t year_l		:8;
	} bits;
#else
	uint8_t h;							// "BCD"
	uint8_t m;							// "BCD"
	uint8_t s;							// "BCD"
	uint8_t day;						// "BCD"
	uint8_t month;						// "BCD"
	uint8_t year_h;						// "BCD"
	uint8_t year_l;						// "BCD"
#endif
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_date_time;





/*
 * IC current time/date & request to set time/date
 * Direction: IC -> LCM_ALL
 */

#define	 CAN_PF_IC_CORRECTION_OF_HEADLIGHTS_TIME_CYCLE_MS					0UL
#define	 CAN_PF_IC_CORRECTION_OF_HEADLIGHTS_SET								0xF0UL

typedef struct
{
#ifdef C2000
	struct
	{
		int16_t  corrector			:8;
		int16_t  corrector_changes	:8;
	} bits;
#else
	int8_t corrector;							// "Corrector persent ", "precision 1",   "range 0 .. 100"
	int8_t corrector_changes; 					// "incremental Corrector" "precision 1", "range -100 .. 100"
#endif
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_corrector_control;





/*
 * IC current time/date & request to set time/date
 * Direction: CAN_SA_LCM_LFR -> IC
 * Direction: CAN_SA_LCM_RFR -> IC
 */

#define	 CAN_PF_LCM_CORRECTION_OF_HEADLIGHTS_TIME_CYCLE_MS					1000UL
#define	 CAN_PF_LCM_CORRECTION_OF_HEADLIGHTS_STATUS							0x01UL

typedef struct
{
#ifdef C2000
	struct
	{
		int16_t  corrector			:8;
		int16_t  reserv				:8;
	} bits;
#else
	int8_t corrector;							// "%", "precision 1",   "range 0 .. 100"
	int8_t reserv; 								//
#endif
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_ic_corrector_status;


/*
 * BCU control message for LCMs
 * Direction: BCU -> all
 */

#define CAN_PF_BCU_LIGHTS_CTRL_CYCLE_MS						100UL
#define CAN_PF_BCU_LIGHTS_CTRL								0xF0UL
typedef struct							// Broadcast
{
#ifdef C2000
	uint16_t lights;
	struct
	{									// "bit field"
		uint16_t bits_flags		:8;
		uint16_t reserv			:8;
	} bits;
#else
	uint16_t lights;									// "bit field"
	uint8_t bits_flags;									// "bit field"
	uint8_t reserv;
#endif
}

#define CAN_PF_BCU_LIGHTS_LOW_BEAM_LAMP						0x01UL
#define CAN_PF_BCU_LIGHTS_HIGH_BEAM_LAMP					0x04UL
#define CAN_PF_BCU_LIGHTS_MARKER_LIGHT_LAMP					0x10UL
#define CAN_PF_BCU_LIGHTS_LEFT_TURN_LAMP					0x40UL
#define CAN_PF_BCU_LIGHTS_RIGHT_TURN_LAMP					0x100UL
#define CAN_PF_BCU_LIGHTS_FRONT_FOG_LAMP					0x400UL
#define CAN_PF_BCU_LIGHTS_REAR_FOG_LAMP						0x1000UL


#define CAN_PF_BCU_LIGHTS_LEFT_SLEEP_MARKER_FLAG			0x01UL
#define CAN_PF_BCU_LIGHTS_RIGHT_SLEEP_MARKER_FLAG			0x02UL
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_bcu_lights_ctrl;




/*
 * BCU control message for WCMs
 * Direction: BCU -> all
 */

#define	 CAN_PF_BCU_WIPERS_CTRL_CYCLE_MS					100UL
#define CAN_PF_BCU_WIPERS_CTRL								0xF1UL
typedef struct							// Broadcast
{
#ifdef C2000
	struct
	{
		uint16_t wiper_front					:8;
		uint16_t wiper_front_int_state			:8;
		uint16_t washer_state					:8; //bit field
		uint16_t wiper_rear						:8;
	} bits;
#else
	uint8_t wiper_front;
	uint8_t wiper_front_int_state;
	uint8_t washer_state; //bit field
	uint8_t wiper_rear;
#endif
}
//WIPER FRONT STATE
#define CAN_PF_BCU_WIPERS_FRONT_OFF 	0x0UL
#define CAN_PF_BCU_WIPERS_FRONT_INT 	0x1UL
#define CAN_PF_BCU_WIPERS_FRONT_LOW 	0x2UL
#define CAN_PF_BCU_WIPERS_FRONT_HIGH 	0x3UL
//WIPER FRONT INT STATE
#define CAN_PF_BCU_WIPERS_FRONT_INT_3SEC 	0x0UL
#define CAN_PF_BCU_WIPERS_FRONT_INT_6SEC 	0x1UL
#define CAN_PF_BCU_WIPERS_FRONT_INT_9SEC 	0x2UL
#define CAN_PF_BCU_WIPERS_FRONT_INT_12SEC 	0x3UL
// WASHER  STATE
#define CAN_PF_BCU_WASHER_FRONT 	0x1UL
#define CAN_PF_BCU_WASHER_REAR 		0x2UL
//WIPER REAR STATE
#define CAN_PF_BCU_WIPERS_REAR_OFF 	0x0UL
#define CAN_PF_BCU_WIPERS_REAR_INT 	0x1UL
#define CAN_PF_BCU_WIPERS_REAR_LOW 	0x2UL

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_bcu_wipers_ctrl;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * L_EPW control message for R_EPW
 * Direction: L_EPW -> R_EPW
 */

#define CAN_PF_L_EPW_CTRL_CYCLE_MS						50UL
#define CAN_PF_L_EPW_CTRL											0x01UL
typedef struct
{
	int16_t front_left;
	int16_t front_right;
	uint16_t mirror_message;
	uint16_t lock_unlock_message;
}

#ifndef C2000
__attribute__((__packed__))
#endif
t_can_l_epw_ctrl;




/*
 * EPW limit switch BCAST
 * Direction: EPW -> BCAST
 */

#define CAN_PF_EPW_LIM_SW_CYCLE_MS				100UL
#define CAN_PF_EPW_LIM_SW							0x02UL
typedef struct
{
	uint16_t limit_switch;    // bit field
}

#define CAN_PF_EPW_LIMIT_SWITCH_READY_TO_OFF 		0x01UL
#define CAN_PF_EPW_DOORS_LIMIT_SWITCH_ONE_ON			0x02UL
#define CAN_PF_EPW_DOORS_LIMIT_SWITCH_TWO_ON			0x04UL
#define CAN_PF_EPW_LOCK_LIMIT_SWITCH_ONE_ON			0x08UL
#define CAN_PF_EPW_LOCK_LIMIT_SWITCH_TWO_ON			0x10UL
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_epw_lim_sw;



/*
 * BCU doorr locks control BCAST L_EPW -> R_EPW
 * Direction: BCU -> BCAST
 */

#define CAN_PF_DOOR_LOCKS_CTRL_CYCLE_MS						100UL
#define CAN_PF_DOOR_LOCKS_CTRL								0xB9UL
typedef struct							// Broadcast
{
#ifdef C2000
	struct
	{
		uint16_t left_doors			:8;
		uint16_t right_doors		:8;
	} bits;
#else
	uint8_t left_doors;
	uint8_t right_doors;
#endif
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_door_locks_ctrl;





/*
 * CAN_SA_BUTTONPNL buttons press/long-press/release events
 * Direction: CAN_SA_BUTTONPNL -> IC
 */

#define CAN_PF_BUTTONPNL_BUTTON_CYCLE_MS			0x00UL
#define CAN_PF_BUTTONPNL_BUTTON                	0x01UL

typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t id   : 8;
		uint16_t type : 8;
	} bits;
#else
	uint8_t id;										// "enumeration"
	uint8_t type;									// "enumeration"
#endif
#define CAN_PF_BUTTONPNL_BUTTON_ID_6				0x00UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_5				0x01UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_4				0x02UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_3				0x03UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_2				0x04UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_1				0x05UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_BOARD_COMP		0x06UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_BACK				0x07UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_NAVIGATOR		0x08UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_RADIO			0x09UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_MUSIC			0x0AUL
#define CAN_PF_BUTTONPNL_BUTTON_ID_INET				0x0BUL
#define CAN_PF_BUTTONPNL_BUTTON_ID_PHONE			0x0CUL
#define CAN_PF_BUTTONPNL_BUTTON_ID_MENU				0x0DUL
#define CAN_PF_BUTTONPNL_BUTTON_ID_FUNCTION			0x0EUL
#define CAN_PF_BUTTONPNL_BUTTON_ID_ENCSELECT		0x0FUL
#define CAN_PF_BUTTONPNL_BUTTON_ID_MUSBACK			0x10UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_MUSFF			0x11UL
#define CAN_PF_BUTTONPNL_BUTTON_ID_MUSSELECT		0x12UL

#define CAN_PF_BUTTONPNL_BUTTON_TYPE_PRESS			0x01UL
#define CAN_PF_BUTTONPNL_BUTTON_TYPE_RELEASE		0x02UL
#define CAN_PF_BUTTONPNL_BUTTON_TYPE_LONG_PRESS		0x03UL
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_buttonpnl_button;


/*
 * CP encoder events
 * Direction: CP -> IC
 */

#define CAN_PF_BUTTONPNL_ENCODER_CYCLE_MS				0x00UL
#define CAN_PF_BUTTONPNL_ENCODER                       0x02UL


typedef struct
{
#ifdef C2000
	struct
	{
		uint16_t shift   		:	8;
		uint16_t direction 		:	8;
	} bits;
#else
	uint8_t shift;		    						// "1", "precision 1"
	uint8_t direction;	    						// "enumeration"
#endif
#define CAN_PF_BUTTONPNL_ENCODER_DIRECTION_POS		0x00UL
#define CAN_PF_BUTTONPNL_ENCODER_DIRECTION_NEG		0x01UL
}
#ifndef C2000
__attribute__((__packed__))
#endif
t_can_buttonpnl_encoder;


#endif //_DCU_EMC_H_



