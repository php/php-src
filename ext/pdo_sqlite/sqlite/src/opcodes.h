/* Automatically generated.  Do not edit */
/* See the mkopcodeh.awk script for details */
#define OP_VNext                                1
#define OP_Column                               2
#define OP_SetCookie                            3
#define OP_Real                               125   /* same as TK_FLOAT    */
#define OP_Sequence                             4
#define OP_MoveGt                               5
#define OP_Ge                                  72   /* same as TK_GE       */
#define OP_RowKey                               6
#define OP_SCopy                                7
#define OP_Eq                                  68   /* same as TK_EQ       */
#define OP_OpenWrite                            8
#define OP_NotNull                             66   /* same as TK_NOTNULL  */
#define OP_If                                   9
#define OP_ToInt                              141   /* same as TK_TO_INT   */
#define OP_String8                             88   /* same as TK_STRING   */
#define OP_VRowid                              10
#define OP_CollSeq                             11
#define OP_OpenRead                            12
#define OP_Expire                              13
#define OP_AutoCommit                          14
#define OP_Gt                                  69   /* same as TK_GT       */
#define OP_IntegrityCk                         15
#define OP_Sort                                17
#define OP_Copy                                18
#define OP_Trace                               19
#define OP_Function                            20
#define OP_IfNeg                               21
#define OP_And                                 61   /* same as TK_AND      */
#define OP_Subtract                            79   /* same as TK_MINUS    */
#define OP_Noop                                22
#define OP_Return                              23
#define OP_Remainder                           82   /* same as TK_REM      */
#define OP_NewRowid                            24
#define OP_Multiply                            80   /* same as TK_STAR     */
#define OP_Variable                            25
#define OP_String                              26
#define OP_RealAffinity                        27
#define OP_VRename                             28
#define OP_ParseSchema                         29
#define OP_VOpen                               30
#define OP_Close                               31
#define OP_CreateIndex                         32
#define OP_IsUnique                            33
#define OP_NotFound                            34
#define OP_Int64                               35
#define OP_MustBeInt                           36
#define OP_Halt                                37
#define OP_Rowid                               38
#define OP_IdxLT                               39
#define OP_AddImm                              40
#define OP_Statement                           41
#define OP_RowData                             42
#define OP_MemMax                              43
#define OP_Or                                  60   /* same as TK_OR       */
#define OP_NotExists                           44
#define OP_Gosub                               45
#define OP_Divide                              81   /* same as TK_SLASH    */
#define OP_Integer                             46
#define OP_ToNumeric                          140   /* same as TK_TO_NUMERIC*/
#define OP_Prev                                47
#define OP_Concat                              83   /* same as TK_CONCAT   */
#define OP_BitAnd                              74   /* same as TK_BITAND   */
#define OP_VColumn                             48
#define OP_CreateTable                         49
#define OP_Last                                50
#define OP_IsNull                              65   /* same as TK_ISNULL   */
#define OP_IncrVacuum                          51
#define OP_IdxRowid                            52
#define OP_ShiftRight                          77   /* same as TK_RSHIFT   */
#define OP_ResetCount                          53
#define OP_FifoWrite                           54
#define OP_ContextPush                         55
#define OP_DropTrigger                         56
#define OP_DropIndex                           57
#define OP_IdxGE                               58
#define OP_IdxDelete                           59
#define OP_Vacuum                              62
#define OP_MoveLe                              63
#define OP_IfNot                               64
#define OP_DropTable                           73
#define OP_MakeRecord                          84
#define OP_ToBlob                             139   /* same as TK_TO_BLOB  */
#define OP_ResultRow                           85
#define OP_Delete                              86
#define OP_AggFinal                            89
#define OP_ShiftLeft                           76   /* same as TK_LSHIFT   */
#define OP_Goto                                90
#define OP_TableLock                           91
#define OP_FifoRead                            92
#define OP_Clear                               93
#define OP_MoveLt                              94
#define OP_Le                                  70   /* same as TK_LE       */
#define OP_VerifyCookie                        95
#define OP_AggStep                             96
#define OP_ToText                             138   /* same as TK_TO_TEXT  */
#define OP_Not                                 16   /* same as TK_NOT      */
#define OP_ToReal                             142   /* same as TK_TO_REAL  */
#define OP_SetNumColumns                       97
#define OP_Transaction                         98
#define OP_VFilter                             99
#define OP_Ne                                  67   /* same as TK_NE       */
#define OP_VDestroy                           100
#define OP_ContextPop                         101
#define OP_BitOr                               75   /* same as TK_BITOR    */
#define OP_Next                               102
#define OP_IdxInsert                          103
#define OP_Lt                                  71   /* same as TK_LT       */
#define OP_Insert                             104
#define OP_Destroy                            105
#define OP_ReadCookie                         106
#define OP_ForceInt                           107
#define OP_LoadAnalysis                       108
#define OP_Explain                            109
#define OP_OpenPseudo                         110
#define OP_OpenEphemeral                      111
#define OP_Null                               112
#define OP_Move                               113
#define OP_Blob                               114
#define OP_Add                                 78   /* same as TK_PLUS     */
#define OP_Rewind                             115
#define OP_MoveGe                             116
#define OP_VBegin                             117
#define OP_VUpdate                            118
#define OP_IfZero                             119
#define OP_BitNot                              87   /* same as TK_BITNOT   */
#define OP_VCreate                            120
#define OP_Found                              121
#define OP_IfPos                              122
#define OP_NullRow                            123

/* The following opcode values are never used */
#define OP_NotUsed_124                        124
#define OP_NotUsed_126                        126
#define OP_NotUsed_127                        127
#define OP_NotUsed_128                        128
#define OP_NotUsed_129                        129
#define OP_NotUsed_130                        130
#define OP_NotUsed_131                        131
#define OP_NotUsed_132                        132
#define OP_NotUsed_133                        133
#define OP_NotUsed_134                        134
#define OP_NotUsed_135                        135
#define OP_NotUsed_136                        136
#define OP_NotUsed_137                        137


/* Properties such as "out2" or "jump" that are specified in
** comments following the "case" for each opcode in the vdbe.c
** are encoded into bitvectors as follows:
*/
#define OPFLG_JUMP            0x0001  /* jump:  P2 holds jmp target */
#define OPFLG_OUT2_PRERELEASE 0x0002  /* out2-prerelease: */
#define OPFLG_IN1             0x0004  /* in1:   P1 is an input */
#define OPFLG_IN2             0x0008  /* in2:   P2 is an input */
#define OPFLG_IN3             0x0010  /* in3:   P3 is an input */
#define OPFLG_OUT3            0x0020  /* out3:  P3 is an output */
#define OPFLG_INITIALIZER {\
/*   0 */ 0x00, 0x01, 0x00, 0x10, 0x02, 0x11, 0x02, 0x00,\
/*   8 */ 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,\
/*  16 */ 0x04, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,\
/*  24 */ 0x02, 0x02, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00,\
/*  32 */ 0x02, 0x11, 0x11, 0x02, 0x05, 0x00, 0x02, 0x11,\
/*  40 */ 0x04, 0x00, 0x02, 0x0c, 0x11, 0x01, 0x02, 0x01,\
/*  48 */ 0x00, 0x02, 0x01, 0x01, 0x02, 0x00, 0x04, 0x00,\
/*  56 */ 0x00, 0x00, 0x11, 0x08, 0x2c, 0x2c, 0x00, 0x11,\
/*  64 */ 0x05, 0x05, 0x05, 0x15, 0x15, 0x15, 0x15, 0x15,\
/*  72 */ 0x15, 0x00, 0x2c, 0x2c, 0x2c, 0x2c, 0x2c, 0x2c,\
/*  80 */ 0x2c, 0x2c, 0x2c, 0x2c, 0x00, 0x00, 0x00, 0x04,\
/*  88 */ 0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x00,\
/*  96 */ 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x08,\
/* 104 */ 0x00, 0x02, 0x02, 0x05, 0x00, 0x00, 0x00, 0x00,\
/* 112 */ 0x02, 0x00, 0x02, 0x01, 0x11, 0x00, 0x00, 0x05,\
/* 120 */ 0x00, 0x11, 0x05, 0x00, 0x00, 0x02, 0x00, 0x00,\
/* 128 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
/* 136 */ 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04,}
