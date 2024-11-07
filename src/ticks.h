// ICT headers.

#pragma once

#include "sys_dependencies.h"

enum
{
    kICT_SubTick,
#if EmClassicKbrd
    kICT_Kybd_ReceiveCommand,
    kICT_Kybd_ReceiveEndCommand,
#endif
#if EmADB
    kICT_ADB_NewState,
#endif
#if EmPMU
    kICT_PMU_Task,
#endif
    kICT_VIA1_Timer1Check,
    kICT_VIA1_Timer2Check,
#if EmVIA2
    kICT_VIA2_Timer1Check,
    kICT_VIA2_Timer2Check,
#endif

    kNumICTs
};

// #define iCountt uint32_t
typedef uint32_t iCountt;
namespace ICT
{
    extern uimr active;
    extern iCountt when[kNumICTs];
    extern iCountt NextiCount;

    extern void zap(void);
    extern iCountt GetCuriCount(void);
    extern void add(int taskid, uint32_t n);
    extern uint32_t DoGetNext(uint32_t maxn);
    extern void DoCurrentTasks(void);
};

