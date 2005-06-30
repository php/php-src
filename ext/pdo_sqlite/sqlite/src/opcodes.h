/* Automatically generated.  Do not edit */
/* See the mkopcodeh.awk script for details */
#define OP_MemLoad                              1
#define OP_HexBlob                            129   /* same as TK_BLOB     */
#define OP_Column                               2
#define OP_SetCookie                            3
#define OP_IfMemPos                             4
#define OP_Real                               128   /* same as TK_FLOAT    */
#define OP_MoveGt                               5
#define OP_Ge                                  75   /* same as TK_GE       */
#define OP_AggFocus                             6
#define OP_RowKey                               7
#define OP_AggNext                              8
#define OP_Eq                                  71   /* same as TK_EQ       */
#define OP_OpenWrite                            9
#define OP_NotNull                             69   /* same as TK_NOTNULL  */
#define OP_If                                  10
#define OP_String8                             90   /* same as TK_STRING   */
#define OP_Pop                                 11
#define OP_AggContextPush                      12
#define OP_CollSeq                             13
#define OP_OpenRead                            14
#define OP_Expire                              15
#define OP_SortReset                           16
#define OP_AutoCommit                          17
#define OP_Gt                                  72   /* same as TK_GT       */
#define OP_Sort                                18
#define OP_ListRewind                          19
#define OP_IntegrityCk                         20
#define OP_SortInsert                          21
#define OP_Function                            22
#define OP_And                                 63   /* same as TK_AND      */
#define OP_Subtract                            82   /* same as TK_MINUS    */
#define OP_Noop                                23
#define OP_Return                              24
#define OP_Remainder                           85   /* same as TK_REM      */
#define OP_NewRowid                            25
#define OP_Multiply                            83   /* same as TK_STAR     */
#define OP_Variable                            26
#define OP_String                              27
#define OP_ParseSchema                         28
#define OP_AggFunc                             29
#define OP_Close                               30
#define OP_ListWrite                           31
#define OP_CreateIndex                         32
#define OP_IsUnique                            33
#define OP_IdxIsNull                           34
#define OP_NotFound                            35
#define OP_MustBeInt                           36
#define OP_Halt                                37
#define OP_Rowid                               38
#define OP_IdxLT                               39
#define OP_AddImm                              40
#define OP_Statement                           41
#define OP_RowData                             42
#define OP_MemMax                              43
#define OP_Push                                44
#define OP_Or                                  62   /* same as TK_OR       */
#define OP_NotExists                           45
#define OP_OpenTemp                            46
#define OP_MemIncr                             47
#define OP_Gosub                               48
#define OP_Divide                              84   /* same as TK_SLASH    */
#define OP_AggSet                              49
#define OP_Integer                             50
#define OP_SortNext                            51
#define OP_Prev                                52
#define OP_Concat                              86   /* same as TK_CONCAT   */
#define OP_BitAnd                              77   /* same as TK_BITAND   */
#define OP_CreateTable                         53
#define OP_Last                                54
#define OP_IsNull                              68   /* same as TK_ISNULL   */
#define OP_IdxRowid                            55
#define OP_ShiftRight                          80   /* same as TK_RSHIFT   */
#define OP_ResetCount                          56
#define OP_Callback                            57
#define OP_ContextPush                         58
#define OP_DropTrigger                         59
#define OP_DropIndex                           60
#define OP_IdxGE                               61
#define OP_IdxDelete                           65
#define OP_Vacuum                              66
#define OP_MoveLe                              67
#define OP_IfNot                               76
#define OP_DropTable                           88
#define OP_MakeRecord                          91
#define OP_Delete                              92
#define OP_AggContextPop                       93
#define OP_ListRead                            94
#define OP_ListReset                           95
#define OP_ShiftLeft                           79   /* same as TK_LSHIFT   */
#define OP_Dup                                 96
#define OP_Goto                                97
#define OP_Clear                               98
#define OP_IdxGT                               99
#define OP_MoveLt                             100
#define OP_Le                                  73   /* same as TK_LE       */
#define OP_VerifyCookie                       101
#define OP_Pull                               102
#define OP_Not                                 64   /* same as TK_NOT      */
#define OP_SetNumColumns                      103
#define OP_AbsValue                           104
#define OP_Transaction                        105
#define OP_Negative                            87   /* same as TK_UMINUS   */
#define OP_Ne                                  70   /* same as TK_NE       */
#define OP_AggGet                             106
#define OP_ContextPop                         107
#define OP_BitOr                               78   /* same as TK_BITOR    */
#define OP_Next                               108
#define OP_AggInit                            109
#define OP_IdxInsert                          110
#define OP_Distinct                           111
#define OP_Lt                                  74   /* same as TK_LT       */
#define OP_AggReset                           112
#define OP_Insert                             113
#define OP_Destroy                            114
#define OP_ReadCookie                         115
#define OP_ForceInt                           116
#define OP_OpenPseudo                         117
#define OP_Null                               118
#define OP_Blob                               119
#define OP_Add                                 81   /* same as TK_PLUS     */
#define OP_MemStore                           120
#define OP_Rewind                             121
#define OP_MoveGe                             122
#define OP_BitNot                              89   /* same as TK_BITNOT   */
#define OP_Found                              123
#define OP_NullRow                            124

/* The following opcode values are never used */
#define OP_NotUsed_125                        125
#define OP_NotUsed_126                        126
#define OP_NotUsed_127                        127

#define NOPUSH_MASK_0 65400
#define NOPUSH_MASK_1 61871
#define NOPUSH_MASK_2 64446
#define NOPUSH_MASK_3 65363
#define NOPUSH_MASK_4 65535
#define NOPUSH_MASK_5 46015
#define NOPUSH_MASK_6 64254
#define NOPUSH_MASK_7 7987
#define NOPUSH_MASK_8 0
#define NOPUSH_MASK_9 0
