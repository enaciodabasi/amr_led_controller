float64 current_position
float64 current_velocity

float64 target_position
float64 target_velocity

### Driver State Constants:

uint8 NOT_READY_TO_SWITCH_ON = 0
uint8 SWITCH_ON_DISABLED = 1
uint8 READY_TO_SWITCH_ON = 2
uint8 SWITCHED_ON = 3
uint8 OPERATION_ENABLED = 4
uint8 QUICK_STOP_ACTIVE = 5
uint8 FAULT_REACTION_ACTIVE = 6
uint8 FAULT = 7

uint8 status