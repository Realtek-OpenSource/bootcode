/**************************************************************
// Spec Version                  :
// Parser Version                :
// CModelGen Version             :
// Naming Rule                   :
// Naming Rule                   :
// Parse Option                  :
// Parse Address Region          :
// Decode bit number             :
// Firmware Header Generate Date :
***************************************************************/

#define ISO_TCWCR                                                                        0x18007680
#define ISO_TCWCR_reg_addr                                                               "0x18007680"
#define ISO_TCWCR_reg                                                                    0x18007680
#define ISO_set_TCWCR_reg(data)   (*((volatile unsigned int*) TCWCR_reg)=data)
#define ISO_get_TCWCR_reg   (*((volatile unsigned int*) TCWCR_reg))
#define ISO_TCWCR_inst_adr                                                               "0x00A0"
#define ISO_TCWCR_inst                                                                   0x00A0
#define ISO_TCWCR_WD_INT_EN_shift                                                        (31)
#define ISO_TCWCR_WD_INT_EN_mask                                                         (0x80000000)
#define ISO_TCWCR_WD_INT_EN(data)                                                        (0x80000000&((data)<<31))
#define ISO_TCWCR_WD_INT_EN_src(data)                                                    ((0x80000000&(data))>>31)
#define ISO_TCWCR_get_WD_INT_EN(data)                                                    ((0x80000000&(data))>>31)
#define ISO_TCWCR_NMIC_shift                                                             (12)
#define ISO_TCWCR_NMIC_mask                                                              (0x0000F000)
#define ISO_TCWCR_NMIC(data)                                                             (0x0000F000&((data)<<12))
#define ISO_TCWCR_NMIC_src(data)                                                         ((0x0000F000&(data))>>12)
#define ISO_TCWCR_get_NMIC(data)                                                         ((0x0000F000&(data))>>12)
#define ISO_TCWCR_WDC_shift                                                              (8)
#define ISO_TCWCR_WDC_mask                                                               (0x00000F00)
#define ISO_TCWCR_WDC(data)                                                              (0x00000F00&((data)<<8))
#define ISO_TCWCR_WDC_src(data)                                                          ((0x00000F00&(data))>>8)
#define ISO_TCWCR_get_WDC(data)                                                          ((0x00000F00&(data))>>8)
#define ISO_TCWCR_WDEN_shift                                                             (0)
#define ISO_TCWCR_WDEN_mask                                                              (0x000000FF)
#define ISO_TCWCR_WDEN(data)                                                             (0x000000FF&((data)<<0))
#define ISO_TCWCR_WDEN_src(data)                                                         ((0x000000FF&(data))>>0)
#define ISO_TCWCR_get_WDEN(data)                                                         ((0x000000FF&(data))>>0)

#define ISO_TCWTR                                                                        0x18007684
#define ISO_TCWTR_reg_addr                                                               "0x18007684"
#define ISO_TCWTR_reg                                                                    0x18007684
#define ISO_set_TCWTR_reg(data)   (*((volatile unsigned int*) TCWTR_reg)=data)
#define ISO_get_TCWTR_reg   (*((volatile unsigned int*) TCWTR_reg))
#define ISO_TCWTR_inst_adr                                                               "0x00A1"
#define ISO_TCWTR_inst                                                                   0x00A1
#define ISO_TCWTR_WDCLR_shift                                                            (0)
#define ISO_TCWTR_WDCLR_mask                                                             (0x00000001)
#define ISO_TCWTR_WDCLR(data)                                                            (0x00000001&((data)<<0))
#define ISO_TCWTR_WDCLR_src(data)                                                        ((0x00000001&(data))>>0)
#define ISO_TCWTR_get_WDCLR(data)                                                        ((0x00000001&(data))>>0)

#define ISO_TCWOV                                                                        0x1800768C
#define ISO_TCWOV_reg_addr                                                               "0x1800768C"
#define ISO_TCWOV_reg                                                                    0x1800768C
#define ISO_set_TCWOV_reg(data)   (*((volatile unsigned int*) TCWOV_reg)=data)
#define ISO_get_TCWOV_reg   (*((volatile unsigned int*) TCWOV_reg))
#define ISO_TCWOV_inst_adr                                                               "0x00A3"
#define ISO_TCWOV_inst                                                                   0x00A3
#define ISO_TCWOV_SEL_shift                                                              (0)
#define ISO_TCWOV_SEL_mask                                                               (0xFFFFFFFF)
#define ISO_TCWOV_SEL(data)                                                              (0xFFFFFFFF&((data)<<0))
#define ISO_TCWOV_SEL_src(data)                                                          ((0xFFFFFFFF&(data))>>0)
#define ISO_TCWOV_get_SEL(data)                                                          ((0xFFFFFFFF&(data))>>0)
