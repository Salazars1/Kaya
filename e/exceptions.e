#ifndef exceptions
#define exceptions

#include "../h/types.h"
EXTERN void Syscall1(state_t *caller);
EXTERN void Syscall2();
EXTERN void Syscall3(state_t *caller);
EXTERN void Syscall4(state_t *caller);
EXTERN void Syscall5(state_t *caller);
EXTERN void Syscall6(state_t *caller);
EXTERN void Syscall7(state_t *caller);
EXTERN void Syscall8(state_t *caller);
EXTERN void CtrlPlusC(state_t *oldState, state_t *newState);

EXTERN void passUpOrDie();
EXTERN void PrgTrapHandler();
EXTERN void TLBTrapHandler();



#endif