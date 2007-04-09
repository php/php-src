/* Automatically generated.  Do not edit */
/* See the mkopcodeh.awk script for details */
#define OP_NotExists                            1
#define OP_Dup                                  2
#define OP_MoveLt                               3
#define OP_Multiply                            80   /* same as TK_STAR     */
#define OP_VCreate                              4
#define OP_BitAnd                              74   /* same as TK_BITAND   */
#define OP_DropTrigger                          5
#define OP_OpenPseudo                           6
#define OP_MemInt                               7
#define OP_IntegrityCk                          8
#define OP_RowKey                               9
#define OP_LoadAnalysis                        10
#define OP_IdxGT                               11
#define OP_Last                                12
#define OP_Subtract                            79   /* same as TK_MINUS    */
#define OP_MemLoad                             13
#define OP_Remainder                           82   /* same as TK_REM      */
#define OP_SetCookie                           14
#define OP_Sequence                            15
#define OP_Pull                                17
#define OP_VUpdate                             18
#define OP_VColumn                             19
#define OP_DropTable                           20
#define OP_MemStore                            21
#define OP_ContextPush                         22
#define OP_NotNull                             66   /* same as TK_NOTNULL  */
#define OP_Rowid                               23
#define OP_Real                               125   /* same as TK_FLOAT    */
#define OP_String8                             88   /* same as TK_STRING   */
#define OP_And                                 61   /* same as TK_AND      */
#define OP_BitNot                              87   /* same as TK_BITNOT   */
#define OP_VFilter                             24
#define OP_NullRow                             25
#define OP_Noop                                26
#define OP_VRowid                              27
#define OP_Ge                                  72   /* same as TK_GE       */
#define OP_HexBlob                            126   /* same as TK_BLOB     */
#define OP_ParseSchema                         28
#define OP_Statement                           29
#define OP_CollSeq                             30
#define OP_ContextPop                          31
#define OP_ToText                             138   /* same as TK_TO_TEXT  */
#define OP_MemIncr                             32
#define OP_MoveGe                              33
#define OP_Eq                                  68   /* same as TK_EQ       */
#define OP_ToNumeric                          140   /* same as TK_TO_NUMERIC*/
#define OP_If                                  34
#define OP_IfNot                               35
#define OP_ShiftRight                          77   /* same as TK_RSHIFT   */
#define OP_Destroy                             36
#define OP_Distinct                            37
#define OP_CreateIndex                         38
#define OP_SetNumColumns                       39
#define OP_Not                                 16   /* same as TK_NOT      */
#define OP_Gt                                  69   /* same as TK_GT       */
#define OP_ResetCount                          40
#define OP_MakeIdxRec                          41
#define OP_Goto                                42
#define OP_IdxDelete                           43
#define OP_MemMove                             44
#define OP_Found                               45
#define OP_MoveGt                              46
#define OP_IfMemZero                           47
#define OP_MustBeInt                           48
#define OP_Prev                                49
#define OP_MemNull                             50
#define OP_AutoCommit                          51
#define OP_String                              52
#define OP_FifoWrite                           53
#define OP_ToInt                              141   /* same as TK_TO_INT   */
#define OP_Return                              54
#define OP_Callback                            55
#define OP_AddImm                              56
#define OP_Function                            57
#define OP_Concat                              83   /* same as TK_CONCAT   */
#define OP_NewRowid                            58
#define OP_Blob                                59
#define OP_IsNull                              65   /* same as TK_ISNULL   */
#define OP_Next                                62
#define OP_ForceInt                            63
#define OP_ReadCookie                          64
#define OP_Halt                                73
#define OP_Expire                              84
#define OP_Or                                  60   /* same as TK_OR       */
#define OP_DropIndex                           86
#define OP_IdxInsert                           89
#define OP_ShiftLeft                           76   /* same as TK_LSHIFT   */
#define OP_FifoRead                            90
#define OP_Column                              91
#define OP_Int64                               92
#define OP_Gosub                               93
#define OP_IfMemNeg                            94
#define OP_RowData                             95
#define OP_BitOr                               75   /* same as TK_BITOR    */
#define OP_MemMax                              96
#define OP_Close                               97
#define OP_ToReal                             142   /* same as TK_TO_REAL  */
#define OP_VerifyCookie                        98
#define OP_IfMemPos                            99
#define OP_Null                               100
#define OP_Integer                            101
#define OP_Transaction                        102
#define OP_Divide                              81   /* same as TK_SLASH    */
#define OP_IdxLT                              103
#define OP_Delete                             104
#define OP_Rewind                             105
#define OP_Push                               106
#define OP_RealAffinity                       107
#define OP_Clear                              108
#define OP_AggStep                            109
#define OP_Explain                            110
#define OP_Vacuum                             111
#define OP_VDestroy                           112
#define OP_IsUnique                           113
#define OP_VOpen                              114
#define OP_AggFinal                           115
#define OP_OpenWrite                          116
#define OP_Negative                            85   /* same as TK_UMINUS   */
#define OP_Le                                  70   /* same as TK_LE       */
#define OP_VNext                              117
#define OP_AbsValue                           118
#define OP_Sort                               119
#define OP_NotFound                           120
#define OP_MoveLe                             121
#define OP_MakeRecord                         122
#define OP_Add                                 78   /* same as TK_PLUS     */
#define OP_Ne                                  67   /* same as TK_NE       */
#define OP_Variable                           123
#define OP_CreateTable                        124
#define OP_Insert                             127
#define OP_IdxGE                              128
#define OP_OpenRead                           129
#define OP_IdxRowid                           130
#define OP_ToBlob                             139   /* same as TK_TO_BLOB  */
#define OP_VBegin                             131
#define OP_TableLock                          132
#define OP_OpenEphemeral                      133
#define OP_Lt                                  71   /* same as TK_LT       */
#define OP_Pop                                134

/* The following opcode values are never used */
#define OP_NotUsed_135                        135
#define OP_NotUsed_136                        136
#define OP_NotUsed_137                        137

/* Opcodes that are guaranteed to never push a value onto the stack
** contain a 1 their corresponding position of the following mask
** set.  See the opcodeNoPush() function in vdbeaux.c  */
#define NOPUSH_MASK_0 0x5c7a
#define NOPUSH_MASK_1 0xf777
#define NOPUSH_MASK_2 0xedaf
#define NOPUSH_MASK_3 0xf1eb
#define NOPUSH_MASK_4 0xfffe
#define NOPUSH_MASK_5 0x62f7
#define NOPUSH_MASK_6 0xbfcf
#define NOPUSH_MASK_7 0x83bf
#define NOPUSH_MASK_8 0x7c7b
#define NOPUSH_MASK_9 0x0000
