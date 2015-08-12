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

#ifndef _MIS_REG_H_INCLUDED_
#define _MIS_REG_H_INCLUDED_

#define TC0TVR                                                                       0x1801B500
#define TC0TVR_reg_addr                                                              "0x1801B500"
#define TC0TVR_reg                                                                   0x1801B500
#define set_TC0TVR_reg(data)   (*((volatile unsigned int*) TC0TVR_reg)=data)
#define get_TC0TVR_reg   (*((volatile unsigned int*) TC0TVR_reg))
#define TC0TVR_inst_adr                                                              "0x0040"
#define TC0TVR_inst                                                                  0x0040
#define TC0TVR_TC0TVR_shift                                                          (0)
#define TC0TVR_TC0TVR_mask                                                           (0xFFFFFFFF)
#define TC0TVR_TC0TVR(data)                                                          (0xFFFFFFFF&((data)<<0))
#define TC0TVR_TC0TVR_src(data)                                                      ((0xFFFFFFFF&(data))>>0)
#define TC0TVR_get_TC0TVR(data)                                                      ((0xFFFFFFFF&(data))>>0)


#define TC1TVR                                                                       0x1801B504
#define TC1TVR_reg_addr                                                              "0x1801B504"
#define TC1TVR_reg                                                                   0x1801B504
#define set_TC1TVR_reg(data)   (*((volatile unsigned int*) TC1TVR_reg)=data)
#define get_TC1TVR_reg   (*((volatile unsigned int*) TC1TVR_reg))
#define TC1TVR_inst_adr                                                              "0x0041"
#define TC1TVR_inst                                                                  0x0041
#define TC1TVR_TC1TVR_shift                                                          (0)
#define TC1TVR_TC1TVR_mask                                                           (0xFFFFFFFF)
#define TC1TVR_TC1TVR(data)                                                          (0xFFFFFFFF&((data)<<0))
#define TC1TVR_TC1TVR_src(data)                                                      ((0xFFFFFFFF&(data))>>0)
#define TC1TVR_get_TC1TVR(data)                                                      ((0xFFFFFFFF&(data))>>0)


#define TC2TVR                                                                       0x1801B508
#define TC2TVR_reg_addr                                                              "0x1801B508"
#define TC2TVR_reg                                                                   0x1801B508
#define set_TC2TVR_reg(data)   (*((volatile unsigned int*) TC2TVR_reg)=data)
#define get_TC2TVR_reg   (*((volatile unsigned int*) TC2TVR_reg))
#define TC2TVR_inst_adr                                                              "0x0042"
#define TC2TVR_inst                                                                  0x0042
#define TC2TVR_TC2TVR_shift                                                          (0)
#define TC2TVR_TC2TVR_mask                                                           (0xFFFFFFFF)
#define TC2TVR_TC2TVR(data)                                                          (0xFFFFFFFF&((data)<<0))
#define TC2TVR_TC2TVR_src(data)                                                      ((0xFFFFFFFF&(data))>>0)
#define TC2TVR_get_TC2TVR(data)                                                      ((0xFFFFFFFF&(data))>>0)


#define TC0CVR                                                                       0x1801B50C
#define TC0CVR_reg_addr                                                              "0x1801B50C"
#define TC0CVR_reg                                                                   0x1801B50C
#define set_TC0CVR_reg(data)   (*((volatile unsigned int*) TC0CVR_reg)=data)
#define get_TC0CVR_reg   (*((volatile unsigned int*) TC0CVR_reg))
#define TC0CVR_inst_adr                                                              "0x0043"
#define TC0CVR_inst                                                                  0x0043
#define TC0CVR_TC0CVR_shift                                                          (0)
#define TC0CVR_TC0CVR_mask                                                           (0xFFFFFFFF)
#define TC0CVR_TC0CVR(data)                                                          (0xFFFFFFFF&((data)<<0))
#define TC0CVR_TC0CVR_src(data)                                                      ((0xFFFFFFFF&(data))>>0)
#define TC0CVR_get_TC0CVR(data)                                                      ((0xFFFFFFFF&(data))>>0)


#define TC1CVR                                                                       0x1801B510
#define TC1CVR_reg_addr                                                              "0x1801B510"
#define TC1CVR_reg                                                                   0x1801B510
#define set_TC1CVR_reg(data)   (*((volatile unsigned int*) TC1CVR_reg)=data)
#define get_TC1CVR_reg   (*((volatile unsigned int*) TC1CVR_reg))
#define TC1CVR_inst_adr                                                              "0x0044"
#define TC1CVR_inst                                                                  0x0044
#define TC1CVR_TC1CVR_shift                                                          (0)
#define TC1CVR_TC1CVR_mask                                                           (0xFFFFFFFF)
#define TC1CVR_TC1CVR(data)                                                          (0xFFFFFFFF&((data)<<0))
#define TC1CVR_TC1CVR_src(data)                                                      ((0xFFFFFFFF&(data))>>0)
#define TC1CVR_get_TC1CVR(data)                                                      ((0xFFFFFFFF&(data))>>0)


#define TC2CVR                                                                       0x1801B514
#define TC2CVR_reg_addr                                                              "0x1801B514"
#define TC2CVR_reg                                                                   0x1801B514
#define set_TC2CVR_reg(data)   (*((volatile unsigned int*) TC2CVR_reg)=data)
#define get_TC2CVR_reg   (*((volatile unsigned int*) TC2CVR_reg))
#define TC2CVR_inst_adr                                                              "0x0045"
#define TC2CVR_inst                                                                  0x0045
#define TC2CVR_TC2VR_shift                                                           (0)
#define TC2CVR_TC2VR_mask                                                            (0xFFFFFFFF)
#define TC2CVR_TC2VR(data)                                                           (0xFFFFFFFF&((data)<<0))
#define TC2CVR_TC2VR_src(data)                                                       ((0xFFFFFFFF&(data))>>0)
#define TC2CVR_get_TC2VR(data)                                                       ((0xFFFFFFFF&(data))>>0)


#define TC0CR                                                                        0x1801B518
#define TC0CR_reg_addr                                                               "0x1801B518"
#define TC0CR_reg                                                                    0x1801B518
#define set_TC0CR_reg(data)   (*((volatile unsigned int*) TC0CR_reg)=data)
#define get_TC0CR_reg   (*((volatile unsigned int*) TC0CR_reg))
#define TC0CR_inst_adr                                                               "0x0046"
#define TC0CR_inst                                                                   0x0046
#define TC0CR_TC0En_shift                                                            (31)
#define TC0CR_TC0En_mask                                                             (0x80000000)
#define TC0CR_TC0En(data)                                                            (0x80000000&((data)<<31))
#define TC0CR_TC0En_src(data)                                                        ((0x80000000&(data))>>31)
#define TC0CR_get_TC0En(data)                                                        ((0x80000000&(data))>>31)
#define TC0CR_TC0Mode_shift                                                          (30)
#define TC0CR_TC0Mode_mask                                                           (0x40000000)
#define TC0CR_TC0Mode(data)                                                          (0x40000000&((data)<<30))
#define TC0CR_TC0Mode_src(data)                                                      ((0x40000000&(data))>>30)
#define TC0CR_get_TC0Mode(data)                                                      ((0x40000000&(data))>>30)
#define TC0CR_TC0Pause_shift                                                         (29)
#define TC0CR_TC0Pause_mask                                                          (0x20000000)
#define TC0CR_TC0Pause(data)                                                         (0x20000000&((data)<<29))
#define TC0CR_TC0Pause_src(data)                                                     ((0x20000000&(data))>>29)
#define TC0CR_get_TC0Pause(data)                                                     ((0x20000000&(data))>>29)
#define TC0CR_RvdA_shift                                                             (24)
#define TC0CR_RvdA_mask                                                              (0x1F000000)
#define TC0CR_RvdA(data)                                                             (0x1F000000&((data)<<24))
#define TC0CR_RvdA_src(data)                                                         ((0x1F000000&(data))>>24)
#define TC0CR_get_RvdA(data)                                                         ((0x1F000000&(data))>>24)


#define TC1CR                                                                        0x1801B51C
#define TC1CR_reg_addr                                                               "0x1801B51C"
#define TC1CR_reg                                                                    0x1801B51C
#define set_TC1CR_reg(data)   (*((volatile unsigned int*) TC1CR_reg)=data)
#define get_TC1CR_reg   (*((volatile unsigned int*) TC1CR_reg))
#define TC1CR_inst_adr                                                               "0x0047"
#define TC1CR_inst                                                                   0x0047
#define TC1CR_TC1En_shift                                                            (31)
#define TC1CR_TC1En_mask                                                             (0x80000000)
#define TC1CR_TC1En(data)                                                            (0x80000000&((data)<<31))
#define TC1CR_TC1En_src(data)                                                        ((0x80000000&(data))>>31)
#define TC1CR_get_TC1En(data)                                                        ((0x80000000&(data))>>31)
#define TC1CR_TC1Mode_shift                                                          (30)
#define TC1CR_TC1Mode_mask                                                           (0x40000000)
#define TC1CR_TC1Mode(data)                                                          (0x40000000&((data)<<30))
#define TC1CR_TC1Mode_src(data)                                                      ((0x40000000&(data))>>30)
#define TC1CR_get_TC1Mode(data)                                                      ((0x40000000&(data))>>30)
#define TC1CR_TC1Pause_shift                                                         (29)
#define TC1CR_TC1Pause_mask                                                          (0x20000000)
#define TC1CR_TC1Pause(data)                                                         (0x20000000&((data)<<29))
#define TC1CR_TC1Pause_src(data)                                                     ((0x20000000&(data))>>29)
#define TC1CR_get_TC1Pause(data)                                                     ((0x20000000&(data))>>29)
#define TC1CR_RvdA_shift                                                             (24)
#define TC1CR_RvdA_mask                                                              (0x1F000000)
#define TC1CR_RvdA(data)                                                             (0x1F000000&((data)<<24))
#define TC1CR_RvdA_src(data)                                                         ((0x1F000000&(data))>>24)
#define TC1CR_get_RvdA(data)                                                         ((0x1F000000&(data))>>24)


#define TC2CR                                                                        0x1801B520
#define TC2CR_reg_addr                                                               "0x1801B520"
#define TC2CR_reg                                                                    0x1801B520
#define set_TC2CR_reg(data)   (*((volatile unsigned int*) TC2CR_reg)=data)
#define get_TC2CR_reg   (*((volatile unsigned int*) TC2CR_reg))
#define TC2CR_inst_adr                                                               "0x0048"
#define TC2CR_inst                                                                   0x0048
#define TC2CR_TC2En_shift                                                            (31)
#define TC2CR_TC2En_mask                                                             (0x80000000)
#define TC2CR_TC2En(data)                                                            (0x80000000&((data)<<31))
#define TC2CR_TC2En_src(data)                                                        ((0x80000000&(data))>>31)
#define TC2CR_get_TC2En(data)                                                        ((0x80000000&(data))>>31)
#define TC2CR_TC2Mode_shift                                                          (30)
#define TC2CR_TC2Mode_mask                                                           (0x40000000)
#define TC2CR_TC2Mode(data)                                                          (0x40000000&((data)<<30))
#define TC2CR_TC2Mode_src(data)                                                      ((0x40000000&(data))>>30)
#define TC2CR_get_TC2Mode(data)                                                      ((0x40000000&(data))>>30)
#define TC2CR_TC2Pause_shift                                                         (29)
#define TC2CR_TC2Pause_mask                                                          (0x20000000)
#define TC2CR_TC2Pause(data)                                                         (0x20000000&((data)<<29))
#define TC2CR_TC2Pause_src(data)                                                     ((0x20000000&(data))>>29)
#define TC2CR_get_TC2Pause(data)                                                     ((0x20000000&(data))>>29)
#define TC2CR_RvdA_shift                                                             (24)
#define TC2CR_RvdA_mask                                                              (0x1F000000)
#define TC2CR_RvdA(data)                                                             (0x1F000000&((data)<<24))
#define TC2CR_RvdA_src(data)                                                         ((0x1F000000&(data))>>24)
#define TC2CR_get_RvdA(data)                                                         ((0x1F000000&(data))>>24)


#define TC0ICR                                                                       0x1801B524
#define TC0ICR_reg_addr                                                              "0x1801B524"
#define TC0ICR_reg                                                                   0x1801B524
#define set_TC0ICR_reg(data)   (*((volatile unsigned int*) TC0ICR_reg)=data)
#define get_TC0ICR_reg   (*((volatile unsigned int*) TC0ICR_reg))
#define TC0ICR_inst_adr                                                              "0x0049"
#define TC0ICR_inst                                                                  0x0049
#define TC0ICR_TC0IE_shift                                                           (31)
#define TC0ICR_TC0IE_mask                                                            (0x80000000)
#define TC0ICR_TC0IE(data)                                                           (0x80000000&((data)<<31))
#define TC0ICR_TC0IE_src(data)                                                       ((0x80000000&(data))>>31)
#define TC0ICR_get_TC0IE(data)                                                       ((0x80000000&(data))>>31)


#define TC1ICR                                                                       0x1801B528
#define TC1ICR_reg_addr                                                              "0x1801B528"
#define TC1ICR_reg                                                                   0x1801B528
#define set_TC1ICR_reg(data)   (*((volatile unsigned int*) TC1ICR_reg)=data)
#define get_TC1ICR_reg   (*((volatile unsigned int*) TC1ICR_reg))
#define TC1ICR_inst_adr                                                              "0x004A"
#define TC1ICR_inst                                                                  0x004A
#define TC1ICR_TC1IE_shift                                                           (31)
#define TC1ICR_TC1IE_mask                                                            (0x80000000)
#define TC1ICR_TC1IE(data)                                                           (0x80000000&((data)<<31))
#define TC1ICR_TC1IE_src(data)                                                       ((0x80000000&(data))>>31)
#define TC1ICR_get_TC1IE(data)                                                       ((0x80000000&(data))>>31)


#define TC2ICR                                                                       0x1801B52C
#define TC2ICR_reg_addr                                                              "0x1801B52C"
#define TC2ICR_reg                                                                   0x1801B52C
#define set_TC2ICR_reg(data)   (*((volatile unsigned int*) TC2ICR_reg)=data)
#define get_TC2ICR_reg   (*((volatile unsigned int*) TC2ICR_reg))
#define TC2ICR_inst_adr                                                              "0x004B"
#define TC2ICR_inst                                                                  0x004B
#define TC2ICR_TC2IE_shift                                                           (31)
#define TC2ICR_TC2IE_mask                                                            (0x80000000)
#define TC2ICR_TC2IE(data)                                                           (0x80000000&((data)<<31))
#define TC2ICR_TC2IE_src(data)                                                       ((0x80000000&(data))>>31)
#define TC2ICR_get_TC2IE(data)                                                       ((0x80000000&(data))>>31)

#endif
