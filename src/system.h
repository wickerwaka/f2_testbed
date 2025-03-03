#if !defined (SYSTEM_H)
#define SYSTEM_H 1

#include "tc0100scn.h"
#include "tc0220ioc.h"
#include "tc0200obj.h"

static volatile uint16_t *TC011PCR_ADDR = (volatile uint16_t *)0x200000;
static volatile uint16_t *TC011PCR_DATA = (volatile uint16_t *)0x200002;
static volatile uint16_t *TC011PCR_WHAT = (volatile uint16_t *)0x200004;

static TC0100SCN_Layout *TC0100SCN = (TC0100SCN_Layout *)0x800000;
static TC0100SCN_Control *TC0100SCN_Ctrl = (TC0100SCN_Control *)0x820000;

static TC0220IOC_Control *TC0220IOC = (TC0220IOC_Control *)0x300000;

static TC0200OBJ_Inst *TC0200OBJ = (TC0200OBJ_Inst *)0x900000;



#endif
