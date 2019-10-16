#include "../h/types.h"
#include "../h/const.h"
#ifndef exceptions
#define exceptions


extern void Syscall1(state_t *caller);
extern void Syscall2();
extern void Syscall3(state_t *caller);
extern void Syscall4(state_t *caller);
extern void Syscall5(state_t *caller);
extern void Syscall6(state_t *caller);
extern void Syscall7(state_t *caller);
extern void Syscall8(state_t *caller);
extern void CtrlPlusC(state_t *oldState, state_t *newState);

extern void passUpOrDie();
extern void PrgTrapHandler();
extern void TLBTrapHandler();
extern void SYSCALLHandler();


#endif