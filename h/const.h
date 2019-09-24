#ifndef CONSTS
#define CONSTS

/**************************************************************************** 
 *
 * This header file contains utility constants & macro definitions.
 * 
 ****************************************************************************/

/* Hardware & software constants */
#define PAGESIZE 4096 /* page size in bytes */
#define WORDLEN 4     /* word size in bytes */
#define PTEMAGICNO 0x2A

#define ROMPAGESTART 0x20000000 /* ROM Reserved Page */

/* timer, timescale, TOD-LO and other bus regs */
#define RAMBASEADDR 0x10000000
#define TODLOADDR 0x1000001C
#define INTERVALTMR 0x10000020
#define TIMESCALEADDR 0x10000024

/* utility constants */
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0
#define HIDDEN static
#define EOS '\0'

#define NULL ((void *)0xFFFFFFFF)

/* vectors number and type */
#define VECTSNUM 4

#define TLBTRAP 0
#define PROGTRAP 1
#define SYSTRAP 2

#define TRAPTYPES 3

/* device interrupts */
#define DISKINT 3
#define TAPEINT 4
#define NETWINT 5
#define PRNTINT 6
#define TERMINT 7

#define DEVREGLEN 4   /* device register field length in bytes & regs per dev */
#define DEVREGSIZE 16 /* device register size in bytes */

/* device register field number for non-terminal devices */
#define STATUS 0
#define COMMAND 1
#define DATA0 2
#define DATA1 3

/* device register field number for terminal devices */
#define RECVSTATUS 0
#define RECVCOMMAND 1
#define TRANSTATUS 2
#define TRANCOMMAND 3

/* device common STATUS codes */
#define UNINSTALLED 0
#define READY 1
#define BUSY 3

/* device common COMMAND codes */
#define RESET 0
#define ACK 1

/* operations */
#define MIN(A, B) ((A) < (B) ? A : B)
#define MAX(A, B) ((A) < (B) ? B : A)
#define ALIGNED(A) (((unsigned)A & 0x3) == 0)

/* Useful operations */
#define STCK(T) ((T) = ((*((cpu_t *)TODLOADDR)) / (*((cpu_t *)TIMESCALEADDR))))
#define LDIT(T) ((*((cpu_t *)INTERVALTMR)) = (T) * (*((cpu_t *)TIMESCALEADDR)))

#define MAXPROC 20
#define MAXINT 0xFFFFFFFF
#define QUANTUM 5000


/*  Explanation of the SEMNUM number. (49)
        There are 8 terminals (x2 (each terminal its schizophrenic))
        There are 8 tapes
        There are 8 printers
        There are 8 Networks
        There are 8 disks
        1 timer

        ARR LAYOUT: TERMINAL1a|TERMINAL1b|TAPE1|PRINTER1|NETWORK1|DISK1|TERMINAL2a ... 
        */
#define SEMNUM 49

/* NEW Processor State Areas */
    /* SYSCALL BREAK*/
#define SYSCALLNEWAREA 0x200003D4
#define SYSCALLOLDAREA 0X20000348
    /* PROGRAM TRAP */
#define PRGMTRAPNEWAREA 0x200002BC
#define PRGMTRAPOLDAREA 0x20000230
    /* TLB MANAGEMENT */
#define TBLMGMTNEWAREA 0x200001A4
#define TBLMGMTOLDAREA 0x20000118
    /* INTERRUPTS */
#define INTERRUPTNEWAREA 0x2000008C
#define INTERRUPTOLDAREA 0x20000000

/* ON/OFF bit manipulation*/
#define ALLOFF  0x00000000
#define VMOFF   0xFDFFFFFF     /* Virtual Memory OFF*/
#define VMON    0x02000000     /* Virtual Memory ON*/
#define UMONFF  0xFFFFFFF7     /* User Mode OFF (Kernel Mode ON)*/
#define UMON    0x00000008     /* User Mode ON (Kernel Mode OFF)*/
#define IMOFF   0xFFFF00FF     /* Interrupt UNMasked */
#define IMON    0x0000FF00     /* Interrupt Masked */


#endif
