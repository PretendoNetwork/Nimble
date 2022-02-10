#ifndef _A_ARCH_WIIU_PPC_H
#define _A_ARCH_WIIU_PPC_H

#include <gctypes.h>

#define DABR_TRANSLATION (1 << 2)
#define DABR_DATA_STORE (1 << 1)
#define DABR_DATA_LOAD (1 << 0)

#define MSR_LE_BIT (1 << 0) // Little-endian mode enable
#define MSR_RI_BIT (1 << 1) // Exeception recoverable enable
#define MSR_PM_BIT (1 << 2) // Performance Monitor enable
#define MSR_R0_BIT (1 << 3) // Reserved
#define MSR_DR_BIT (1 << 4) // Data address translation enable
#define MSR_IR_BIT (1 << 5) // Instruction address translation enable
#define MSR_IP_BIT \
    (1 << 6)                 // Exception prefix, 0 -> fetch from 0x000n_nnnn | 1 -> fetch from
                             // 0xFFFn_nnnn
#define MSR_R1_BIT (1 << 7)  // Reserved
#define MSR_FE1_BIT (1 << 8) // IEEE floating-point exception mode 1
#define MSR_BE_BIT \
    (1 << 9)                  // Branch trace enable (generates a branch type trace exception when
                              // a branch instruction executes successfully)
#define MSR_SE_BIT (1 << 10)  // Single-step trace enable (except rfi, isync, and sc)
#define MSR_FE0_BIT (1 << 11) // IEEE floating-point exception mode 0
#define MSR_ME_BIT (1 << 12)  // Machine check enable
#define MSR_FP_BIT (1 << 13)  // Floating-point available
#define MSR_PR_BIT (1 << 14)  // Privilege level | 0 -> supervisor mode, 1 -> user mode
#define MSR_EE_BIT (1 << 15)  // External interrupt enable
#define MSR_ILE_BIT (1 << 16) // Exception little-endian mode
#define MSR_R2_BIT (1 << 17)  // Reserved
#define MSR_POW_BIT (1 << 18) // Power management enable

// The remaining bits are reserved.
// 19 -> 21: Reserved, partial function
// 22 -> 26: Reserved, full function
// 27 -> 30: Reserved, partial function
// 31      : Reserved, full function

#define SR_CUSTOM_VSID 0x00AABBCC

#define SR_T_BIT (1 << 31)
#define SR_KS_BIT (1 << 30)
#define SR_KS_SHIFT 30
#define SR_KP_BIT (1 << 29)
#define SR_KP_SHIFT 29
#define SR_NX_BIT (1 << 28)
#define SR_NX_SHIFT 28

#define SR_VSID_MASK 0xFFFFFF

#define PTEH_V_BIT (1 << 31)
#define PTEH_VSID_MASK 0xFFFFFF
#define PTEH_VSID_SHIFT 7
#define PTEH_H_BIT (1 << 6)
#define PTEH_API_MASK 0x3F

#define PTEL_RPN_MASK 0xFFFFF000
#define PTEL_R_BIT (1 << 8)
#define PTEL_C_BIT (1 << 7)
#define PTEL_WIMG_MASK 15
#define PTEL_WIMG_SHIFT 3
#define PTEL_PP_MASK 3

#define BAT_AREA_128K 0x0
#define BAT_AREA_256K 0x1
#define BAT_AREA_512K 0x3
#define BAT_AREA_1M 0x7
#define BAT_AREA_2M 0xF
#define BAT_AREA_4M 0x1F
#define BAT_AREA_8M 0x3F
#define BAT_AREA_16M 0x7F
#define BAT_AREA_32M 0xFF
#define BAT_AREA_64M 0x1FF
#define BAT_AREA_128M 0x3FF
#define BAT_AREA_256M 0x7FF

#define BATU_BEPI_MASK 0xFFFE0000
#define BATU_BL_MASK 0x7FF
#define BATU_BL_SHIFT 2
#define BATU_VS_BIT (1 << 1) // Can supervisor access this? (compared against MSR_PR bit)
#define BATU_VP_BIT (1 << 0) // Can the user access this? (compared against MSR_PR bit)

#define BATL_BRPN_MASK 0xFFFE0000
#define BATL_WIMG_MASK 0xF
#define BATL_WIMG_SHIFT 3
#define BATL_PP_MASK 3

#define SDR1_HTABORG_MASK 0xFFFF0000
#define SDR1_HTABORG_SHIFT 16

#define SDR1_HTABMASK_MASK 0x1FF
#define SDR1_HTABMASK_8M 0
#define SDR1_HTABMASK_16M 1
#define SDR1_HTABMASK_32M 3
#define SDR1_HTABMASK_64M 7
#define SDR1_HTABMASK_128M 15
#define SDR1_HTABMASK_256M 31
#define SDR1_HTABMASK_512M 63
#define SDR1_HTABMASK_1G 127
#define SDR1_HTABMASK_2G 255
#define SDR1_HTABMASK_4G 511

#define SPR_SDR1 25

#define SPR_DBAT0U 536
#define SPR_DBAT0L 537
#define SPR_DBAT1U 538
#define SPR_DBAT1L 539
#define SPR_DBAT2U 540
#define SPR_DBAT2L 541
#define SPR_DBAT3U 542
#define SPR_DBAT3L 543
#define SPR_DBAT4U 568
#define SPR_DBAT4L 569
#define SPR_DBAT5U 570
#define SPR_DBAT5L 571
#define SPR_DBAT6U 572
#define SPR_DBAT6L 573
#define SPR_DBAT7U 574
#define SPR_DBAT7L 575

#define SPR_IBAT0U 528
#define SPR_IBAT0L 529
#define SPR_IBAT1U 530
#define SPR_IBAT1L 531
#define SPR_IBAT2U 532
#define SPR_IBAT2L 533
#define SPR_IBAT3U 534
#define SPR_IBAT3L 535
#define SPR_IBAT4U 560
#define SPR_IBAT4L 561
#define SPR_IBAT5U 562
#define SPR_IBAT5L 563
#define SPR_IBAT6U 564
#define SPR_IBAT6L 565
#define SPR_IBAT7U 566
#define SPR_IBAT7L 567

#define SPR_LR 8
#define SPR_CTR 9

#define gpr0 0
#define gpr1 1
#define gpr2 2
#define gpr3 3
#define gpr4 4
#define gpr5 5
#define gpr6 6
#define gpr7 7
#define gpr8 8
#define gpr9 9
#define gpr10 10
#define gpr11 11
#define gpr12 12
#define gpr13 13
#define gpr14 14
#define gpr15 15
#define gpr16 16
#define gpr17 17
#define gpr18 18
#define gpr19 19
#define gpr20 20
#define gpr21 21
#define gpr22 22
#define gpr23 23
#define gpr24 24
#define gpr25 25
#define gpr26 26
#define gpr27 27
#define gpr28 28
#define gpr29 29
#define gpr30 30
#define gpr31 31

#define GPR_MASK 31

inline uint32_t PowerPC_BL(uint32_t dest, uint32_t source) {
    return (0x48000001 | ((dest - source) & 0x03FFFFFC));
}
inline uint32_t PowerPC_BLA(uint32_t absolute_address) {
    return (0x48000003 | (absolute_address & 0x03FFFFFC));
}
inline uint32_t PowerPC_MFMSR(uint32_t RS) {
    return ((31 << 26) | (RS << 21) | (83 << 1));
}
inline uint32_t PowerPC_MTMSR(uint32_t RS) {
    return ((31 << 26) | (RS << 21) | (146 << 1));
}
inline uint32_t PowerPC_MTSPR(uint32_t RS, uint32_t SPR) {
    return ((31 << 26) | (RS << 21) | (SPR << 16) | (467 << 1));
}
inline uint32_t PowerPC_MFSPR(uint32_t RT, uint32_t SPR) {
    return ((31 << 26) | (RT << 21) | (SPR << 16) | (339 << 1));
}
inline uint32_t PowerPC_LWZ(uint32_t RT, uint32_t RA, uint16_t D) {
    return ((32 << 26) | (RT << 21) | (RA << 16) | (D & 0xFFFF));
}
inline uint32_t PowerPC_LBZU(uint32_t RT, uint32_t RA, uint16_t D) {
    return ((35 << 26) | (RT << 21) | (RA << 16) | (D & 0xFFFF));
}
inline uint32_t PowerPC_STW(uint32_t RS, uint32_t RA, uint16_t D) {
    return ((36 << 26) | (RS << 21) | (RA << 16) | (D & 0xFFFF));
}
inline uint32_t PowerPC_STBU(uint32_t RS, uint32_t RA, uint16_t D) {
    return ((39 << 26) | (RS << 21) | (RA << 16) | (D & 0xFFFF));
}
inline uint32_t PowerPC_LI(uint32_t RS, uint32_t VL) {
    return ((14 << 26) | (RS << 21) | (VL & 0xFFFF));
}
inline uint32_t PowerPC_LIS(uint32_t RS, uint32_t VL) {
    return ((15 << 26) | (RS << 21) | (VL & 0xFFFF));
}
inline uint32_t PowerPC_ANDC(uint32_t RS, uint32_t RA, uint32_t RB) {
    return ((31 << 26) | (RS << 21) | (RA << 16) | (RB << 11) | (60 << 1));
}
inline uint32_t PowerPC_ADDI(uint32_t RT, uint32_t RA, uint16_t SI) {
    return ((14 << 26) | (RT << 21) | (RA << 16) | (SI & 0xFFFF));
}
inline uint32_t PowerPC_BDNZ(uint32_t D, uint32_t AA, uint32_t LK) {
    return ((16 << 26) | (16 << 21) | ((D << 16) & 0xFFFF) | (AA << 1) | LK);
}
inline uint32_t PowerPC_ORI(uint32_t RS, uint32_t RA, uint16_t UI) {
    return ((24 << 26) | (RS << 21) | (RA << 16) | (UI & 0xFFFF));
}

#endif