/* Don't edit the following defines. They are the part of opcode specializer */

#undef OP1_TYPE_PREFIX
#if OP1 == IS_CONST
# define OP1_TYPE_PREFIX _CONST
#elif OP1 == IS_TMP_VAR
# define OP1_TYPE_PREFIX _TMP
#elif OP1 == IS_VAR
# define OP1_TYPE_PREFIX _VAR
#elif OP1 == IS_UNUSED
# define OP1_TYPE_PREFIX _UNUSED
#elif OP1 == IS_ANY
# define OP1_TYPE_PREFIX _ANY
#else
# error "Wrong OP1_TYPE!"
#endif

#undef OP2_TYPE_PREFIX
#if OP2 == IS_CONST
# define OP2_TYPE_PREFIX _CONST
#elif OP2 == IS_TMP_VAR
# define OP2_TYPE_PREFIX _TMP
#elif OP2 == IS_VAR
# define OP2_TYPE_PREFIX _VAR
#elif OP2 == IS_UNUSED
# define OP2_TYPE_PREFIX _UNUSED
#elif OP2 == IS_ANY
# define OP2_TYPE_PREFIX _ANY
#else
# error "Wrong OP2_TYPE!"
#endif

/* ordered list of opcode handlers */

#define ZEND_VM_LABELS \
	ZEND_VM_LABEL(ZEND_NOP),                     /*   0 */ \
	ZEND_VM_LABEL(ZEND_ADD),                     /*   1 */ \
	ZEND_VM_LABEL(ZEND_SUB),                     /*   2 */ \
	ZEND_VM_LABEL(ZEND_MUL),                     /*   3 */ \
	ZEND_VM_LABEL(ZEND_DIV),                     /*   4 */ \
	ZEND_VM_LABEL(ZEND_MOD),                     /*   5 */ \
	ZEND_VM_LABEL(ZEND_SL),                      /*   6 */ \
	ZEND_VM_LABEL(ZEND_SR),                      /*   7 */ \
	ZEND_VM_LABEL(ZEND_CONCAT),                  /*   8 */ \
	ZEND_VM_LABEL(ZEND_BW_OR),                   /*   9 */ \
	ZEND_VM_LABEL(ZEND_BW_AND),                  /*  10 */ \
	ZEND_VM_LABEL(ZEND_BW_XOR),                  /*  11 */ \
	ZEND_VM_LABEL(ZEND_BW_NOT),                  /*  12 */ \
	ZEND_VM_LABEL(ZEND_BOOL_NOT),                /*  13 */ \
	ZEND_VM_LABEL(ZEND_BOOL_XOR),                /*  14 */ \
	ZEND_VM_LABEL(ZEND_IS_IDENTICAL),            /*  15 */ \
	ZEND_VM_LABEL(ZEND_IS_NOT_IDENTICAL),        /*  16 */ \
	ZEND_VM_LABEL(ZEND_IS_EQUAL),                /*  17 */ \
	ZEND_VM_LABEL(ZEND_IS_NOT_EQUAL),            /*  18 */ \
	ZEND_VM_LABEL(ZEND_IS_SMALLER),              /*  19 */ \
	ZEND_VM_LABEL(ZEND_IS_SMALLER_OR_EQUAL),     /*  20 */ \
	ZEND_VM_LABEL(ZEND_CAST),                    /*  21 */ \
	ZEND_VM_LABEL(ZEND_QM_ASSIGN),               /*  22 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_ADD),              /*  23 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_SUB),              /*  24 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_MUL),              /*  25 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_DIV),              /*  26 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_MOD),              /*  27 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_SL),               /*  28 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_SR),               /*  29 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_CONCAT),           /*  30 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_BW_OR),            /*  31 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_BW_AND),           /*  32 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_BW_XOR),           /*  33 */ \
	ZEND_VM_LABEL(ZEND_PRE_INC),                 /*  34 */ \
	ZEND_VM_LABEL(ZEND_PRE_DEC),                 /*  35 */ \
	ZEND_VM_LABEL(ZEND_POST_INC),                /*  36 */ \
	ZEND_VM_LABEL(ZEND_POST_DEC),                /*  37 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN),                  /*  38 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_REF),              /*  39 */ \
	ZEND_VM_LABEL(ZEND_ECHO),                    /*  40 */ \
	ZEND_VM_LABEL(ZEND_PRINT),                   /*  41 */ \
	ZEND_VM_LABEL(ZEND_JMP),                     /*  42 */ \
	ZEND_VM_LABEL(ZEND_JMPZ),                    /*  43 */ \
	ZEND_VM_LABEL(ZEND_JMPNZ),                   /*  44 */ \
	ZEND_VM_LABEL(ZEND_JMPZNZ),                  /*  45 */ \
	ZEND_VM_LABEL(ZEND_JMPZ_EX),                 /*  46 */ \
	ZEND_VM_LABEL(ZEND_JMPNZ_EX),                /*  47 */ \
	ZEND_VM_LABEL(ZEND_CASE),                    /*  48 */ \
	ZEND_VM_LABEL(ZEND_SWITCH_FREE),             /*  49 */ \
	ZEND_VM_LABEL(ZEND_BRK),                     /*  50 */ \
	ZEND_VM_LABEL(ZEND_CONT),                    /*  51 */ \
	ZEND_VM_LABEL(ZEND_BOOL),                    /*  52 */ \
	ZEND_VM_LABEL(ZEND_INIT_STRING),             /*  53 */ \
	ZEND_VM_LABEL(ZEND_ADD_CHAR),                /*  54 */ \
	ZEND_VM_LABEL(ZEND_ADD_STRING),              /*  55 */ \
	ZEND_VM_LABEL(ZEND_ADD_VAR),                 /*  56 */ \
	ZEND_VM_LABEL(ZEND_BEGIN_SILENCE),           /*  57 */ \
	ZEND_VM_LABEL(ZEND_END_SILENCE),             /*  58 */ \
	ZEND_VM_LABEL(ZEND_INIT_FCALL_BY_NAME),      /*  59 */ \
	ZEND_VM_LABEL(ZEND_DO_FCALL),                /*  60 */ \
	ZEND_VM_LABEL(ZEND_DO_FCALL_BY_NAME),        /*  61 */ \
	ZEND_VM_LABEL(ZEND_RETURN),                  /*  62 */ \
	ZEND_VM_LABEL(ZEND_RECV),                    /*  63 */ \
	ZEND_VM_LABEL(ZEND_RECV_INIT),               /*  64 */ \
	ZEND_VM_LABEL(ZEND_SEND_VAL),                /*  65 */ \
	ZEND_VM_LABEL(ZEND_SEND_VAR),                /*  66 */ \
	ZEND_VM_LABEL(ZEND_SEND_REF),                /*  67 */ \
	ZEND_VM_LABEL(ZEND_NEW),                     /*  68 */ \
	ZEND_VM_LABEL(ZEND_JMP_NO_CTOR),             /*  69 */ \
	ZEND_VM_LABEL(ZEND_FREE),                    /*  70 */ \
	ZEND_VM_LABEL(ZEND_INIT_ARRAY),              /*  71 */ \
	ZEND_VM_LABEL(ZEND_ADD_ARRAY_ELEMENT),       /*  72 */ \
	ZEND_VM_LABEL(ZEND_INCLUDE_OR_EVAL),         /*  73 */ \
	ZEND_VM_LABEL(ZEND_UNSET_VAR),               /*  74 */ \
	ZEND_VM_LABEL(ZEND_UNSET_DIM_OBJ),           /*  75 */ \
	ZEND_VM_NULL_LABEL,                          /*  76 */ \
	ZEND_VM_LABEL(ZEND_FE_RESET),                /*  77 */ \
	ZEND_VM_LABEL(ZEND_FE_FETCH),                /*  78 */ \
	ZEND_VM_LABEL(ZEND_EXIT),                    /*  79 */ \
	ZEND_VM_LABEL(ZEND_FETCH_R),                 /*  80 */ \
	ZEND_VM_LABEL(ZEND_FETCH_DIM_R),             /*  81 */ \
	ZEND_VM_LABEL(ZEND_FETCH_OBJ_R),             /*  82 */ \
	ZEND_VM_LABEL(ZEND_FETCH_W),                 /*  83 */ \
	ZEND_VM_LABEL(ZEND_FETCH_DIM_W),             /*  84 */ \
	ZEND_VM_LABEL(ZEND_FETCH_OBJ_W),             /*  85 */ \
	ZEND_VM_LABEL(ZEND_FETCH_RW),                /*  86 */ \
	ZEND_VM_LABEL(ZEND_FETCH_DIM_RW),            /*  87 */ \
	ZEND_VM_LABEL(ZEND_FETCH_OBJ_RW),            /*  88 */ \
	ZEND_VM_LABEL(ZEND_FETCH_IS),                /*  89 */ \
	ZEND_VM_LABEL(ZEND_FETCH_DIM_IS),            /*  90 */ \
	ZEND_VM_LABEL(ZEND_FETCH_OBJ_IS),            /*  91 */ \
	ZEND_VM_LABEL(ZEND_FETCH_FUNC_ARG),          /*  92 */ \
	ZEND_VM_LABEL(ZEND_FETCH_DIM_FUNC_ARG),      /*  93 */ \
	ZEND_VM_LABEL(ZEND_FETCH_OBJ_FUNC_ARG),      /*  94 */ \
	ZEND_VM_LABEL(ZEND_FETCH_UNSET),             /*  95 */ \
	ZEND_VM_LABEL(ZEND_FETCH_DIM_UNSET),         /*  96 */ \
	ZEND_VM_LABEL(ZEND_FETCH_OBJ_UNSET),         /*  97 */ \
	ZEND_VM_LABEL(ZEND_FETCH_DIM_TMP_VAR),       /*  98 */ \
	ZEND_VM_LABEL(ZEND_FETCH_CONSTANT),          /*  99 */ \
	ZEND_VM_NULL_LABEL,                          /* 100 */ \
	ZEND_VM_LABEL(ZEND_EXT_STMT),                /* 101 */ \
	ZEND_VM_LABEL(ZEND_EXT_FCALL_BEGIN),         /* 102 */ \
	ZEND_VM_LABEL(ZEND_EXT_FCALL_END),           /* 103 */ \
	ZEND_VM_LABEL(ZEND_EXT_NOP),                 /* 104 */ \
	ZEND_VM_LABEL(ZEND_TICKS),                   /* 105 */ \
	ZEND_VM_LABEL(ZEND_SEND_VAR_NO_REF),         /* 106 */ \
	ZEND_VM_LABEL(ZEND_CATCH),                   /* 107 */ \
	ZEND_VM_LABEL(ZEND_THROW),                   /* 108 */ \
	ZEND_VM_LABEL(ZEND_FETCH_CLASS),             /* 109 */ \
	ZEND_VM_LABEL(ZEND_CLONE),                   /* 110 */ \
	ZEND_VM_LABEL(ZEND_INIT_CTOR_CALL),          /* 111 */ \
	ZEND_VM_LABEL(ZEND_INIT_METHOD_CALL),        /* 112 */ \
	ZEND_VM_LABEL(ZEND_INIT_STATIC_METHOD_CALL), /* 113 */ \
	ZEND_VM_LABEL(ZEND_ISSET_ISEMPTY_VAR),       /* 114 */ \
	ZEND_VM_LABEL(ZEND_ISSET_ISEMPTY_DIM_OBJ),   /* 115 */ \
	ZEND_VM_NULL_LABEL,                          /* 116 */ \
	ZEND_VM_NULL_LABEL,                          /* 117 */ \
	ZEND_VM_NULL_LABEL,                          /* 118 */ \
	ZEND_VM_NULL_LABEL,                          /* 119 */ \
	ZEND_VM_NULL_LABEL,                          /* 120 */ \
	ZEND_VM_NULL_LABEL,                          /* 121 */ \
	ZEND_VM_NULL_LABEL,                          /* 122 */ \
	ZEND_VM_NULL_LABEL,                          /* 123 */ \
	ZEND_VM_NULL_LABEL,                          /* 124 */ \
	ZEND_VM_NULL_LABEL,                          /* 125 */ \
	ZEND_VM_NULL_LABEL,                          /* 126 */ \
	ZEND_VM_NULL_LABEL,                          /* 127 */ \
	ZEND_VM_NULL_LABEL,                          /* 128 */ \
	ZEND_VM_NULL_LABEL,                          /* 129 */ \
	ZEND_VM_NULL_LABEL,                          /* 130 */ \
	ZEND_VM_NULL_LABEL,                          /* 131 */ \
	ZEND_VM_LABEL(ZEND_PRE_INC_OBJ),             /* 132 */ \
	ZEND_VM_LABEL(ZEND_PRE_DEC_OBJ),             /* 133 */ \
	ZEND_VM_LABEL(ZEND_POST_INC_OBJ),            /* 134 */ \
	ZEND_VM_LABEL(ZEND_POST_DEC_OBJ),            /* 135 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_OBJ),              /* 136 */ \
	ZEND_VM_NULL_LABEL,                          /* 137 */ \
	ZEND_VM_LABEL(ZEND_INSTANCEOF),              /* 138 */ \
	ZEND_VM_LABEL(ZEND_DECLARE_CLASS),           /* 139 */ \
	ZEND_VM_LABEL(ZEND_DECLARE_INHERITED_CLASS), /* 140 */ \
	ZEND_VM_LABEL(ZEND_DECLARE_FUNCTION),        /* 141 */ \
	ZEND_VM_LABEL(ZEND_RAISE_ABSTRACT_ERROR),    /* 142 */ \
	ZEND_VM_NULL_LABEL,                          /* 143 */ \
	ZEND_VM_LABEL(ZEND_ADD_INTERFACE),           /* 144 */ \
	ZEND_VM_NULL_LABEL,                          /* 145 */ \
	ZEND_VM_LABEL(ZEND_VERIFY_ABSTRACT_CLASS),   /* 146 */ \
	ZEND_VM_LABEL(ZEND_ASSIGN_DIM),              /* 147 */ \
	ZEND_VM_LABEL(ZEND_ISSET_ISEMPTY_PROP_OBJ),  /* 148 */ \
	ZEND_VM_LABEL(ZEND_HANDLE_EXCEPTION),        /* 149 */ \
	ZEND_VM_NULL_LABEL

/* opcode handlers and helpers */

#define ZEND_ADD_SPEC() OPDEF(ZEND_ADD, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ADD)
ZEND_VM_HANDLER(ZEND_ADD)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_SUB_SPEC() OPDEF(ZEND_SUB, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_SUB)
ZEND_VM_HANDLER(ZEND_SUB)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_MUL_SPEC() OPDEF(ZEND_MUL, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_MUL)
ZEND_VM_HANDLER(ZEND_MUL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_DIV_SPEC() OPDEF(ZEND_DIV, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_DIV)
ZEND_VM_HANDLER(ZEND_DIV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_MOD_SPEC() OPDEF(ZEND_MOD, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_MOD)
ZEND_VM_HANDLER(ZEND_MOD)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_SL_SPEC() OPDEF(ZEND_SL, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_SL)
ZEND_VM_HANDLER(ZEND_SL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_SR_SPEC() OPDEF(ZEND_SR, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_SR)
ZEND_VM_HANDLER(ZEND_SR)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_CONCAT_SPEC() OPDEF(ZEND_CONCAT, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_CONCAT)
ZEND_VM_HANDLER(ZEND_CONCAT)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_IS_IDENTICAL_SPEC() OPDEF(ZEND_IS_IDENTICAL, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_IS_IDENTICAL)
ZEND_VM_HANDLER(ZEND_IS_IDENTICAL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_IS_NOT_IDENTICAL_SPEC() OPDEF(ZEND_IS_NOT_IDENTICAL, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_IS_NOT_IDENTICAL)
ZEND_VM_HANDLER(ZEND_IS_NOT_IDENTICAL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_IS_EQUAL_SPEC() OPDEF(ZEND_IS_EQUAL, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_IS_EQUAL)
ZEND_VM_HANDLER(ZEND_IS_EQUAL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_IS_NOT_EQUAL_SPEC() OPDEF(ZEND_IS_NOT_EQUAL, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_IS_NOT_EQUAL)
ZEND_VM_HANDLER(ZEND_IS_NOT_EQUAL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_IS_SMALLER_SPEC() OPDEF(ZEND_IS_SMALLER, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_IS_SMALLER)
ZEND_VM_HANDLER(ZEND_IS_SMALLER)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_IS_SMALLER_OR_EQUAL_SPEC() OPDEF(ZEND_IS_SMALLER_OR_EQUAL, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_IS_SMALLER_OR_EQUAL)
ZEND_VM_HANDLER(ZEND_IS_SMALLER_OR_EQUAL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BW_OR_SPEC() OPDEF(ZEND_BW_OR, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_BW_OR)
ZEND_VM_HANDLER(ZEND_BW_OR)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BW_AND_SPEC() OPDEF(ZEND_BW_AND, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_BW_AND)
ZEND_VM_HANDLER(ZEND_BW_AND)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BW_XOR_SPEC() OPDEF(ZEND_BW_XOR, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_BW_XOR)
ZEND_VM_HANDLER(ZEND_BW_XOR)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BOOL_XOR_SPEC() OPDEF(ZEND_BOOL_XOR, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_BOOL_XOR)
ZEND_VM_HANDLER(ZEND_BOOL_XOR)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BW_NOT_SPEC() OPDEF(ZEND_BW_NOT, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_BW_NOT)
ZEND_VM_HANDLER(ZEND_BW_NOT)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_not_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BOOL_NOT_SPEC() OPDEF(ZEND_BOOL_NOT, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_BOOL_NOT)
ZEND_VM_HANDLER(ZEND_BOOL_NOT)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	boolean_not_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#if OP1_OP2_MASK(M_VAR_UNUSED, M_CONST_TMP_VAR)
ZEND_VM_HELPER_EX(zend_binary_assign_op_obj_helper, int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC))
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1, free_op2, free_op_data1;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	zval *object;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	zval tmp;
	znode *result = &opline->result;
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (object->type != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		FREE_OP2();
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
	} else {
		/* here we are sure we are dealing with an object */
		switch (OP2_TYPE) {
			case IS_CONST:
				/* already a constant string */
				break;
			case IS_VAR:
				tmp = *property;
				zval_copy_ctor(&tmp);
				convert_to_string(&tmp);
				property = &tmp;
				break;
			case IS_TMP_VAR:
				convert_to_string(property);
				break;
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_RW TSRMLS_CC);
					break;
				case ZEND_ASSIGN_DIM:
					z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_RW TSRMLS_CC);
					break;
			}
			if (z->type == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			binary_op(z, z, value TSRMLS_CC);
			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
					break;
				case ZEND_ASSIGN_DIM:
					Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
					break;
			}
			if (!RETURN_VALUE_UNUSED(result)) {
				*retval = z;
				PZVAL_LOCK(*retval);
			}
			zval_ptr_dtor(&z);
		}

		if (property == &tmp) {
			zval_dtor(property);
		}

		FREE_OP2();
		FREE_OP(free_op_data1);
	}

	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_binary_assign_op_helper, int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC))
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, binary_op);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);

				(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */

				if ((*object_ptr)->type == IS_OBJECT) {
					ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, binary_op);
				} else {
					zend_op *op_data = opline+1;

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW), GET_OP2_ZVAL_PTR(BP_VAR_R), BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
				}
			}
			break;
		default:
			value = GET_OP2_ZVAL_PTR(BP_VAR_R);
			var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP2();
		FREE_OP1_VAR_PTR();
		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	FREE_OP2();

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
		FREE_OP_VAR_PTR(free_op_data2);
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ASSIGN_ADD_SPEC() OPDEF(ZEND_ASSIGN_ADD, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_ADD)
ZEND_VM_HANDLER(ZEND_ASSIGN_ADD)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, add_function);
}
#endif

#define ZEND_ASSIGN_SUB_SPEC() OPDEF(ZEND_ASSIGN_SUB, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_SUB)
ZEND_VM_HANDLER(ZEND_ASSIGN_SUB)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, sub_function);
}
#endif

#define ZEND_ASSIGN_MUL_SPEC() OPDEF(ZEND_ASSIGN_MUL, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_MUL)
ZEND_VM_HANDLER(ZEND_ASSIGN_MUL)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, mul_function);
}
#endif

#define ZEND_ASSIGN_DIV_SPEC() OPDEF(ZEND_ASSIGN_DIV, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_DIV)
ZEND_VM_HANDLER(ZEND_ASSIGN_DIV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, div_function);
}
#endif

#define ZEND_ASSIGN_MOD_SPEC() OPDEF(ZEND_ASSIGN_MOD, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_MOD)
ZEND_VM_HANDLER(ZEND_ASSIGN_MOD)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, mod_function);
}
#endif

#define ZEND_ASSIGN_SL_SPEC() OPDEF(ZEND_ASSIGN_SL, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_SL)
ZEND_VM_HANDLER(ZEND_ASSIGN_SL)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, shift_left_function);
}
#endif

#define ZEND_ASSIGN_SR_SPEC() OPDEF(ZEND_ASSIGN_SR, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_SR)
ZEND_VM_HANDLER(ZEND_ASSIGN_SR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, shift_right_function);
}
#endif

#define ZEND_ASSIGN_CONCAT_SPEC() OPDEF(ZEND_ASSIGN_CONCAT, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_CONCAT)
ZEND_VM_HANDLER(ZEND_ASSIGN_CONCAT)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, concat_function);
}
#endif

#define ZEND_ASSIGN_BW_OR_SPEC() OPDEF(ZEND_ASSIGN_BW_OR, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_BW_OR)
ZEND_VM_HANDLER(ZEND_ASSIGN_BW_OR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_or_function);
}
#endif

#define ZEND_ASSIGN_BW_AND_SPEC() OPDEF(ZEND_ASSIGN_BW_AND, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_BW_AND)
ZEND_VM_HANDLER(ZEND_ASSIGN_BW_AND)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_and_function);
}
#endif

#define ZEND_ASSIGN_BW_XOR_SPEC() OPDEF(ZEND_ASSIGN_BW_XOR, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_BW_XOR)
ZEND_VM_HANDLER(ZEND_ASSIGN_BW_XOR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_xor_function);
}
#endif

#if OP1_OP2_MASK(M_VAR_UNUSED, M_CONST_TMP_VAR)
ZEND_VM_HELPER_EX(zend_pre_incdec_property_helper, incdec_t incdec_op)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	zval *object;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (object->type != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		FREE_OP2();
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_RW TSRMLS_CC);

		if (z->type == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
			zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

			if (z->refcount == 0) {
				zval_dtor(z);
				FREE_ZVAL(z);
			}
			z = value;
		}
		z->refcount++;
		SEPARATE_ZVAL_IF_NOT_REF(&z);
		incdec_op(z);
		*retval = z;
		Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		zval_ptr_dtor(&z);
	}

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_PRE_INC_OBJ_SPEC() OPDEF(ZEND_PRE_INC_OBJ, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_PRE_INC_OBJ)
ZEND_VM_HANDLER(ZEND_PRE_INC_OBJ)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_pre_incdec_property_helper, incdec_op, increment_function);
}
#endif

#define ZEND_PRE_DEC_OBJ_SPEC() OPDEF(ZEND_PRE_DEC_OBJ, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_PRE_DEC_OBJ)
ZEND_VM_HANDLER(ZEND_PRE_DEC_OBJ)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_pre_incdec_property_helper, incdec_op, decrement_function);
}
#endif

#if OP1_OP2_MASK(M_VAR_UNUSED, M_CONST_TMP_VAR)
ZEND_VM_HELPER_EX(zend_post_incdec_property_helper, incdec_t incdec_op)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	zval *object;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (object->type != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		FREE_OP2();
		*retval = *EG(uninitialized_zval_ptr);
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_RW TSRMLS_CC);

		if (z->type == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
			zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

			if (z->refcount == 0) {
				zval_dtor(z);
				FREE_ZVAL(z);
			}
			z = value;
		}
		*retval = *z;
		zendi_zval_copy_ctor(*retval);
		incdec_op(z);
		z->refcount++;
		Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
		zval_ptr_dtor(&z);
	}

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_POST_INC_OBJ_SPEC() OPDEF(ZEND_POST_INC_OBJ, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_POST_INC_OBJ)
ZEND_VM_HANDLER(ZEND_POST_INC_OBJ)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_post_incdec_property_helper, incdec_op, increment_function);
}
#endif

#define ZEND_POST_DEC_OBJ_SPEC() OPDEF(ZEND_POST_DEC_OBJ, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_POST_DEC_OBJ)
ZEND_VM_HANDLER(ZEND_POST_DEC_OBJ)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_post_incdec_property_helper, incdec_op, decrement_function);
}
#endif

#define ZEND_PRE_INC_SPEC() OPDEF(ZEND_PRE_INC, M_VAR, M_ANY)
#if HAVE_OP(ZEND_PRE_INC)
ZEND_VM_HANDLER(ZEND_PRE_INC)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_PRE_DEC_SPEC() OPDEF(ZEND_PRE_DEC, M_VAR, M_ANY)
#if HAVE_OP(ZEND_PRE_DEC)
ZEND_VM_HANDLER(ZEND_PRE_DEC)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_POST_INC_SPEC() OPDEF(ZEND_POST_INC, M_VAR, M_ANY)
#if HAVE_OP(ZEND_POST_INC)
ZEND_VM_HANDLER(ZEND_POST_INC)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	EX_T(opline->result.u.var).tmp_var = **var_ptr;
	zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_POST_DEC_SPEC() OPDEF(ZEND_POST_DEC, M_VAR, M_ANY)
#if HAVE_OP(ZEND_POST_DEC)
ZEND_VM_HANDLER(ZEND_POST_DEC)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	EX_T(opline->result.u.var).tmp_var = **var_ptr;
	zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ECHO_SPEC() OPDEF(ZEND_ECHO, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_ECHO)
ZEND_VM_HANDLER_EX(ZEND_ECHO)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval z_copy;
	zval *z = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get_method != NULL &&
		zend_std_cast_object_tostring(z, &z_copy, IS_STRING, 0 TSRMLS_CC) == SUCCESS) {
		zend_print_variable(&z_copy);
		zval_dtor(&z_copy);
	} else {
		zend_print_variable(z);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_PRINT_SPEC() OPDEF(ZEND_PRINT, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_PRINT)
ZEND_VM_HANDLER(ZEND_PRINT)
{
	zend_op *opline = EX(opline);

	EX_T(opline->result.u.var).tmp_var.value.lval = 1;
	EX_T(opline->result.u.var).tmp_var.type = IS_LONG;

	ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ECHO);
}
#endif

#if OP1_OP2_MASK(M_CONST_TMP_VAR, M_ANY)
ZEND_VM_HELPER_EX(zend_fetch_var_address_helper, int type)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varname = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval **retval;
	zval tmp_varname;
	HashTable *target_symbol_table;

 	if (varname->type != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		retval = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 0 TSRMLS_CC);
	} else {
		if (opline->op2.u.EA.type == ZEND_FETCH_GLOBAL && opline->op1.op_type == IS_VAR) {
			varname->refcount++;
		}
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), type, varname TSRMLS_CC);
/*
		if (!target_symbol_table) {
			ZEND_VM_NEXT_OPCODE();
		}
*/
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &retval) == FAILURE) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE,"Undefined variable:  %s", varname->value.str.val);
					/* break missing intentionally */
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval_ptr);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable:  %s", varname->value.str.val);
					/* break missing intentionally */
				case BP_VAR_W: {
						zval *new_zval = &EG(uninitialized_zval);

						new_zval->refcount++;
						zend_hash_update(target_symbol_table, varname->value.str.val, varname->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					}
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		switch (opline->op2.u.EA.type) {
			case ZEND_FETCH_GLOBAL:
			case ZEND_FETCH_LOCAL:
				FREE_OP1();
				break;
			case ZEND_FETCH_STATIC:
				zval_update_constant(retval, (void*) 1 TSRMLS_CC);
				break;
		}
	}


	if (varname == &tmp_varname) {
		zval_dtor(varname);
	}
	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = retval;
		PZVAL_LOCK(*retval);
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_IS:
				AI_USE_PTR(EX_T(opline->result.u.var).var);
				break;
			case BP_VAR_UNSET: {
				zend_free_op free_res;

				PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
				if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
					SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
				}
				PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
				FREE_OP_VAR_PTR(free_res);
				break;
			}
		}
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_R_SPEC() OPDEF(ZEND_FETCH_R, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_FETCH_R)
ZEND_VM_HANDLER(ZEND_FETCH_R)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_R);
}
#endif

#define ZEND_FETCH_W_SPEC() OPDEF(ZEND_FETCH_W, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_FETCH_W)
ZEND_VM_HANDLER(ZEND_FETCH_W)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_W);
}
#endif

#define ZEND_FETCH_RW_SPEC() OPDEF(ZEND_FETCH_RW, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_FETCH_RW)
ZEND_VM_HANDLER(ZEND_FETCH_RW)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_RW);
}
#endif

#define ZEND_FETCH_FUNC_ARG_SPEC() OPDEF(ZEND_FETCH_FUNC_ARG, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_FETCH_FUNC_ARG)
ZEND_VM_HANDLER(ZEND_FETCH_FUNC_ARG)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type,
		ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), EX(opline)->extended_value)?BP_VAR_W:BP_VAR_R);
}
#endif

#define ZEND_FETCH_UNSET_SPEC() OPDEF(ZEND_FETCH_UNSET, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_FETCH_UNSET)
ZEND_VM_HANDLER(ZEND_FETCH_UNSET)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_UNSET);
}
#endif

#define ZEND_FETCH_IS_SPEC() OPDEF(ZEND_FETCH_IS, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_FETCH_IS)
ZEND_VM_HANDLER(ZEND_FETCH_IS)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_IS);
}
#endif

#define ZEND_FETCH_DIM_R_SPEC() OPDEF(ZEND_FETCH_DIM_R, M_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_DIM_R)
ZEND_VM_HANDLER(ZEND_FETCH_DIM_R)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_R), GET_OP2_ZVAL_PTR(BP_VAR_R), BP_VAR_R TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_DIM_W_SPEC() OPDEF(ZEND_FETCH_DIM_W, M_VAR, M_CONST_TMP_VAR_UNUSED)
#if HAVE_OP(ZEND_FETCH_DIM_W)
ZEND_VM_HANDLER(ZEND_FETCH_DIM_W)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_W), GET_OP2_ZVAL_PTR(BP_VAR_R), BP_VAR_W TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_DIM_RW_SPEC() OPDEF(ZEND_FETCH_DIM_RW, M_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_DIM_RW)
ZEND_VM_HANDLER(ZEND_FETCH_DIM_RW)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW), GET_OP2_ZVAL_PTR(BP_VAR_R), BP_VAR_RW TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_DIM_IS_SPEC() OPDEF(ZEND_FETCH_DIM_IS, M_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_DIM_IS)
ZEND_VM_HANDLER(ZEND_FETCH_DIM_IS)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW), GET_OP2_ZVAL_PTR(BP_VAR_R), BP_VAR_IS TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_DIM_FUNC_ARG_SPEC() OPDEF(ZEND_FETCH_DIM_FUNC_ARG, M_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_DIM_FUNC_ARG)
ZEND_VM_HANDLER(ZEND_FETCH_DIM_FUNC_ARG)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(type), GET_OP2_ZVAL_PTR(BP_VAR_R), type TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_DIM_UNSET_SPEC() OPDEF(ZEND_FETCH_DIM_UNSET, M_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_DIM_UNSET)
ZEND_VM_HANDLER(ZEND_FETCH_DIM_UNSET)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_UNSET), GET_OP2_ZVAL_PTR(BP_VAR_R), BP_VAR_UNSET TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#if OP1_OP2_MASK(M_VAR_UNUSED, M_CONST_TMP_VAR)
ZEND_VM_HELPER_EX(zend_fetch_property_address_read_helper, int type)
{
	zend_op *opline = EX(opline);
	zval *container;
	zval **retval;
	zend_free_op free_op1;

	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = GET_OP1_OBJ_ZVAL_PTR(type);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE();
	}


	if (container->type != IS_OBJECT) {
		zend_error(E_NOTICE, "Trying to get property of non-object");
		*retval = EG(uninitialized_zval_ptr);
	} else {
		zend_free_op free_op2;
		zval *offset  = GET_OP2_ZVAL_PTR(BP_VAR_R);
		zval tmp;

		switch (OP2_TYPE) {
			case IS_CONST:
				/* already a constant string */
				break;
			case IS_VAR:
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_string(&tmp);
				offset = &tmp;
				break;
			case IS_TMP_VAR:
				convert_to_string(offset);
				break;
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);
		if (offset == &tmp) {
			zval_dtor(offset);
		}
		FREE_OP2();

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
			ZEND_VM_NEXT_OPCODE();
		}
	}

	SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_OBJ_R_SPEC() OPDEF(ZEND_FETCH_OBJ_R, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_OBJ_R)
ZEND_VM_HANDLER(ZEND_FETCH_OBJ_R)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_property_address_read_helper, type, BP_VAR_R);
}
#endif

#define ZEND_FETCH_OBJ_W_SPEC() OPDEF(ZEND_FETCH_OBJ_W, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_OBJ_W)
ZEND_VM_HANDLER(ZEND_FETCH_OBJ_W)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W), GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_W TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_OBJ_RW_SPEC() OPDEF(ZEND_FETCH_OBJ_RW, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_OBJ_RW)
ZEND_VM_HANDLER(ZEND_FETCH_OBJ_RW)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW), GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_RW TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_OBJ_IS_SPEC() OPDEF(ZEND_FETCH_OBJ_IS, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_OBJ_IS)
ZEND_VM_HANDLER(ZEND_FETCH_OBJ_IS)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_property_address_read_helper, type, BP_VAR_IS);
}
#endif

#define ZEND_FETCH_OBJ_FUNC_ARG_SPEC() OPDEF(ZEND_FETCH_OBJ_FUNC_ARG, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_OBJ_FUNC_ARG)
ZEND_VM_HANDLER(ZEND_FETCH_OBJ_FUNC_ARG)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;

		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W), GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_W TSRMLS_CC);
		FREE_OP2();
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	} else {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_property_address_read_helper, type, BP_VAR_R);
	}
}
#endif

#define ZEND_FETCH_OBJ_UNSET_SPEC() OPDEF(ZEND_FETCH_OBJ_UNSET, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_FETCH_OBJ_UNSET)
ZEND_VM_HANDLER(ZEND_FETCH_OBJ_UNSET)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_res;

	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_R), GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_R TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_DIM_TMP_VAR_SPEC() OPDEF(ZEND_FETCH_DIM_TMP_VAR, M_TMP, M_CONST)
#if HAVE_OP(ZEND_FETCH_DIM_TMP_VAR)
ZEND_VM_HANDLER(ZEND_FETCH_DIM_TMP_VAR)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *container = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (container->type != IS_ARRAY) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {
		zend_free_op free_op2;
		zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

		EX_T(opline->result.u.var).var.ptr_ptr = zend_fetch_dimension_address_inner(container->value.ht, dim, BP_VAR_R TSRMLS_CC);
		SELECTIVE_PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &opline->result);
		FREE_OP2();
	}
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ASSIGN_OBJ_SPEC() OPDEF(ZEND_ASSIGN_OBJ, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN_OBJ)
ZEND_VM_HANDLER(ZEND_ASSIGN_OBJ)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);
	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ASSIGN_DIM_SPEC() OPDEF(ZEND_ASSIGN_DIM, M_VAR_UNUSED, M_CONST_TMP_VAR_UNUSED)
#if HAVE_OP(ZEND_ASSIGN_DIM)
ZEND_VM_HANDLER(ZEND_ASSIGN_DIM)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr;

	if (EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && (*object_ptr)->type == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op2, free_op_data1;
		zval *value;

		if (object_ptr) {
			(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
		}
		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_W), GET_OP2_ZVAL_PTR(BP_VAR_R), BP_VAR_W TSRMLS_CC);
		FREE_OP2();

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
	 	FREE_OP_IF_VAR(free_op_data1);
	}
 	FREE_OP1_VAR_PTR();
	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ASSIGN_SPEC() OPDEF(ZEND_ASSIGN, M_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ASSIGN)
ZEND_VM_HANDLER(ZEND_ASSIGN)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *value = GET_OP2_ZVAL_PTR(BP_VAR_R);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (IS_OP2_TMP_FREE()?IS_TMP_VAR:OP2_TYPE), EX(Ts) TSRMLS_CC);
	/* zend_assign_to_variable() always takes care of op2, never free it! */
 	FREE_OP2_IF_VAR();

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ASSIGN_REF_SPEC() OPDEF(ZEND_ASSIGN_REF, M_VAR, M_VAR)
#if HAVE_OP(ZEND_ASSIGN_REF)
ZEND_VM_HANDLER(ZEND_ASSIGN_REF)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **variable_ptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
	zval **value_ptr_ptr = GET_OP2_ZVAL_PTR_PTR(BP_VAR_W);

	zend_assign_to_variable_reference(variable_ptr_ptr, value_ptr_ptr TSRMLS_CC);

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = variable_ptr_ptr;
		PZVAL_LOCK(*variable_ptr_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	FREE_OP1_VAR_PTR();
	FREE_OP2_VAR_PTR();

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_JMP_SPEC() OPDEF(ZEND_JMP, M_ANY, M_ANY)
#if HAVE_OP(ZEND_JMP)
ZEND_VM_HANDLER(ZEND_JMP)
{
#if DEBUG_ZEND>=2
	printf("Jumping to %d\n", opline->op1.u.opline_num);
#endif
	ZEND_VM_SET_OPCODE(EX(opline)->op1.u.jmp_addr);
	ZEND_VM_CONTINUE(); /* CHECK_ME */
}
#endif

#define ZEND_JMPZ_SPEC() OPDEF(ZEND_JMPZ, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_JMPZ)
ZEND_VM_HANDLER(ZEND_JMPZ)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int ret = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	if (!ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.u.jmp_addr);
		ZEND_VM_CONTINUE_JMP();
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_JMPNZ_SPEC() OPDEF(ZEND_JMPNZ, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_JMPNZ)
ZEND_VM_HANDLER(ZEND_JMPNZ)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int ret = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	if (ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.u.jmp_addr);
		ZEND_VM_CONTINUE_JMP();
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_JMPZNZ_SPEC() OPDEF(ZEND_JMPZNZ, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_JMPZNZ)
ZEND_VM_HANDLER(ZEND_JMPZNZ)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on true to %d\n", opline->extended_value);
#endif
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
	} else {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->op2.u.opline_num]);
		ZEND_VM_CONTINUE_JMP();
	}

}
#endif

#define ZEND_JMPZ_EX_SPEC() OPDEF(ZEND_JMPZ_EX, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_JMPZ_EX)
ZEND_VM_HANDLER(ZEND_JMPZ_EX)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	EX_T(opline->result.u.var).tmp_var.value.lval = retval;
	EX_T(opline->result.u.var).tmp_var.type = IS_BOOL;
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.u.jmp_addr);
		ZEND_VM_CONTINUE_JMP();
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_JMPNZ_EX_SPEC() OPDEF(ZEND_JMPNZ_EX, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_JMPNZ_EX)
ZEND_VM_HANDLER(ZEND_JMPNZ_EX)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	EX_T(opline->result.u.var).tmp_var.value.lval = retval;
	EX_T(opline->result.u.var).tmp_var.type = IS_BOOL;
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.u.jmp_addr);
		ZEND_VM_CONTINUE_JMP();
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FREE_SPEC() OPDEF(ZEND_FREE, M_TMP, M_ANY)
#if HAVE_OP(ZEND_FREE)
ZEND_VM_HANDLER(ZEND_FREE)
{
	zendi_zval_dtor(EX_T(EX(opline)->op1.u.var).tmp_var);
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_INIT_STRING_SPEC() OPDEF(ZEND_INIT_STRING, M_ANY, M_ANY)
#if HAVE_OP(ZEND_INIT_STRING)
ZEND_VM_HANDLER(ZEND_INIT_STRING)
{
	zval *tmp = &EX_T(EX(opline)->result.u.var).tmp_var;

	tmp->value.str.val = emalloc(1);
	tmp->value.str.val[0] = 0;
	tmp->value.str.len = 0;
	tmp->refcount = 1;
	tmp->type = IS_STRING;
	tmp->is_ref = 0;
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ADD_CHAR_SPEC() OPDEF(ZEND_ADD_CHAR, M_TMP, M_CONST)
#if HAVE_OP(ZEND_ADD_CHAR)
ZEND_VM_HANDLER(ZEND_ADD_CHAR)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_char_to_string(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_NA),
		&opline->op2.u.constant);
	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ADD_STRING_SPEC() OPDEF(ZEND_ADD_STRING, M_TMP, M_CONST)
#if HAVE_OP(ZEND_ADD_STRING)
ZEND_VM_HANDLER(ZEND_ADD_STRING)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_string_to_string(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_NA),
		&opline->op2.u.constant);
	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ADD_VAR_SPEC() OPDEF(ZEND_ADD_VAR, M_TMP, M_TMP_VAR)
#if HAVE_OP(ZEND_ADD_VAR)
ZEND_VM_HANDLER(ZEND_ADD_VAR)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *var = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval var_copy;
	int use_copy;

	zend_make_printable_zval(var, &var_copy, &use_copy);
	if (use_copy) {
		var = &var_copy;
	}
	add_string_to_string(	&EX_T(opline->result.u.var).tmp_var,
							GET_OP1_ZVAL_PTR(BP_VAR_NA),
							var);
	if (use_copy) {
		zval_dtor(var);
	}
	/* original comment, possibly problematic:
	 * FREE_OP is missing intentionally here - we're always working on the same temporary variable
	 * (Zeev):  I don't think it's problematic, we only use variables
	 * which aren't affected by FREE_OP(Ts, )'s anyway, unless they're
	 * string offsets or overloaded objects
	 */
	FREE_OP2();

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_CLASS_SPEC() OPDEF(ZEND_FETCH_CLASS, M_ANY, M_CONST_TMP_VAR_UNUSED)
#if HAVE_OP(ZEND_FETCH_CLASS)
ZEND_VM_HANDLER(ZEND_FETCH_CLASS)
{
	zend_op *opline = EX(opline);
	zval *class_name;
	zend_free_op free_op2;


	if (OP2_TYPE == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	}

	class_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

	switch (class_name->type) {
		case IS_OBJECT:
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
			break;
		case IS_STRING:
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), ZEND_FETCH_CLASS_DEFAULT TSRMLS_CC);
			break;
		default:
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
			break;
	}

	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_INIT_CTOR_CALL_SPEC() OPDEF(ZEND_INIT_CTOR_CALL, M_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_INIT_CTOR_CALL)
ZEND_VM_HANDLER(ZEND_INIT_CTOR_CALL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(calling_scope));

	if (OP1_TYPE == IS_VAR) {
		SELECTIVE_PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr, &opline->op1);
	}

	/* We are not handling overloaded classes right now */
	EX(object) = GET_OP1_ZVAL_PTR(BP_VAR_R);

	/* New always returns the object as is_ref=0, therefore, we can just increment the reference count */
	EX(object)->refcount++; /* For $this pointer */

	EX(fbc) = EX(fbc_constructor);

	if (EX(fbc)->type == ZEND_USER_FUNCTION) { /* HACK!! */
		EX(calling_scope) = EX(fbc)->common.scope;
	} else {
		EX(calling_scope) = NULL;
	}

	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_INIT_METHOD_CALL_SPEC() OPDEF(ZEND_INIT_METHOD_CALL, M_TMP_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_INIT_METHOD_CALL)
ZEND_VM_HANDLER(ZEND_INIT_METHOD_CALL)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1, free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(calling_scope));

	function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = function_name->value.str.val;
	function_name_strlen = function_name->value.str.len;

	EX(calling_scope) = EG(scope);

	EX(object) = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R);

	if (EX(object) && EX(object)->type == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	if (EX(fbc)->type == ZEND_USER_FUNCTION) {
		EX(calling_scope) = EX(fbc)->common.scope;
	} else {
		EX(calling_scope) = NULL;
	}

	FREE_OP2();
	FREE_OP1_IF_VAR();

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_INIT_STATIC_METHOD_CALL_SPEC() OPDEF(ZEND_INIT_STATIC_METHOD_CALL, M_ANY, M_CONST_TMP_VAR_UNUSED)
#if HAVE_OP(ZEND_INIT_STATIC_METHOD_CALL)
ZEND_VM_HANDLER(ZEND_INIT_STATIC_METHOD_CALL)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(calling_scope));

	ce = EX_T(opline->op1.u.var).class_entry;
	if(OP2_TYPE != IS_UNUSED) {
		char *function_name_strval;
		int function_name_strlen;
		zend_bool is_const = (OP2_TYPE == IS_CONST);
		zend_free_op free_op2;

		if (is_const) {
			function_name_strval = opline->op2.u.constant.value.str.val;
			function_name_strlen = opline->op2.u.constant.value.str.len;
		} else {
			function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Function name must be a string");
			}
			function_name_strval = zend_str_tolower_dup(function_name->value.str.val, function_name->value.str.len);
			function_name_strlen = function_name->value.str.len;
		}

		EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);

		if (!is_const) {
			efree(function_name_strval);
			FREE_OP2();
		}
	} else {
		if(!ce->constructor) {
			zend_error_noreturn(E_ERROR, "Can not call constructor");
		}
		EX(fbc) = ce->constructor;
	}

	EX(calling_scope) = EX(fbc)->common.scope;

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if ((EX(object) = EG(This))) {
			EX(object)->refcount++;
		}
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_INIT_FCALL_BY_NAME_SPEC() OPDEF(ZEND_INIT_FCALL_BY_NAME, M_ANY, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_INIT_FCALL_BY_NAME)
ZEND_VM_HANDLER(ZEND_INIT_FCALL_BY_NAME)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_function *function;
	char *function_name_strval, *lcname;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(calling_scope));

	if (OP2_TYPE == IS_CONST) {
#ifndef ZEND_VM_SPEC
		free_op2.var = NULL;
#endif
		function_name_strval = opline->op2.u.constant.value.str.val;
		function_name_strlen = opline->op2.u.constant.value.str.len;
	} else {
		function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (Z_TYPE_P(function_name) != IS_STRING) {
			zend_error_noreturn(E_ERROR, "Function name must be a string");
		}
		function_name_strval = function_name->value.str.val;
		function_name_strlen = function_name->value.str.len;
	}

	lcname = zend_str_tolower_dup(function_name_strval, function_name_strlen);
	if (zend_hash_find(EG(function_table), lcname, function_name_strlen+1, (void **) &function)==FAILURE) {
		efree(lcname);
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", function_name_strval);
	}

	efree(lcname);
	FREE_OP2();

	EX(calling_scope) = function->common.scope;
	EX(object) = NULL;

	EX(fbc) = function;

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define zend_do_fcall_common_helper_CONST_ANY zend_do_fcall_common_helper_ANY_ANY

#if OP1_OP2_MASK(M_ANY, M_ANY)
ZEND_VM_HELPER(zend_do_fcall_common_helper)
{
	zend_op *opline = EX(opline);
	zval **original_return_value;
	zend_class_entry *current_scope;
	zval *current_this;
	int return_value_used = RETURN_VALUE_USED(opline);
	zend_bool should_change_scope;

	if (EX(function_state).function->common.fn_flags & ZEND_ACC_ABSTRACT) {
		zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
		ZEND_VM_NEXT_OPCODE(); /* Never reached */
	}

	zend_ptr_stack_2_push(&EG(argument_stack), (void *) opline->extended_value, NULL);

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (EX(function_state).function->type == ZEND_USER_FUNCTION
		|| EX(function_state).function->common.scope) {
		should_change_scope = 1;
		current_this = EG(This);
		EG(This) = EX(object);
		current_scope = EG(scope);
		EG(scope) = EX(calling_scope);
	} else {
		should_change_scope = 0;
	}

	EX_T(opline->result.u.var).var.fcall_returned_reference = 0;

	if (EX(function_state).function->common.scope) {
		if (!EG(This) && !(EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)) {
			int severity;
			char *severity_word;
			if (EX(function_state).function->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				severity_word = "should not";
			} else {
				severity = E_ERROR;
				severity_word = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name, severity_word);
		}
	}
	if (EX(function_state).function->type == ZEND_INTERNAL_FUNCTION) {
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		INIT_ZVAL(*(EX_T(opline->result.u.var).var.ptr));

		if (EX(function_state).function->common.arg_info) {
			zend_uint i=0;
			zval **p;
			ulong arg_count;

			p = (zval **) EG(argument_stack).top_element-2;
			arg_count = (ulong) *p;

			while (arg_count>0) {
				zend_verify_arg_type(EX(function_state).function, ++i, *(p-arg_count) TSRMLS_CC);
				arg_count--;
			}
		}
		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			((zend_internal_function *) EX(function_state).function)->handler(opline->extended_value, EX_T(opline->result.u.var).var.ptr, EX(object), return_value_used TSRMLS_CC);
		} else {
			zend_execute_internal(EXECUTE_DATA, return_value_used TSRMLS_CC);
		}

		EG(current_execute_data) = EXECUTE_DATA;
		EX_T(opline->result.u.var).var.ptr->is_ref = 0;
		EX_T(opline->result.u.var).var.ptr->refcount = 1;
		if (!return_value_used) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	} else if (EX(function_state).function->type == ZEND_USER_FUNCTION) {
		HashTable *calling_symbol_table;

		EX_T(opline->result.u.var).var.ptr = NULL;
		if (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			/*printf("Cache hit!  Reusing %x\n", symtable_cache[symtable_cache_ptr]);*/
			EX(function_state).function_symbol_table = *(EG(symtable_cache_ptr)--);
		} else {
			ALLOC_HASHTABLE(EX(function_state).function_symbol_table);
			zend_hash_init(EX(function_state).function_symbol_table, 0, NULL, ZVAL_PTR_DTOR, 0);
			/*printf("Cache miss!  Initialized %x\n", function_state.function_symbol_table);*/
		}
		calling_symbol_table = EG(active_symbol_table);
		EG(active_symbol_table) = EX(function_state).function_symbol_table;
		original_return_value = EG(return_value_ptr_ptr);
		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = (zend_op_array *) EX(function_state).function;

		zend_execute(EG(active_op_array) TSRMLS_CC);
		EX_T(opline->result.u.var).var.fcall_returned_reference = EG(active_op_array)->return_reference;

		if (return_value_used && !EX_T(opline->result.u.var).var.ptr) {
			if (!EG(exception)) {
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			}
		} else if (!return_value_used && EX_T(opline->result.u.var).var.ptr) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(return_value_ptr_ptr)=original_return_value;
		if (EG(symtable_cache_ptr)>=EG(symtable_cache_limit)) {
			zend_hash_destroy(EX(function_state).function_symbol_table);
			FREE_HASHTABLE(EX(function_state).function_symbol_table);
		} else {
			/* clean before putting into the cache, since clean
			   could call dtors, which could use cached hash */
			zend_hash_clean(EX(function_state).function_symbol_table);
			*(++EG(symtable_cache_ptr)) = EX(function_state).function_symbol_table;
		}
		EG(active_symbol_table) = calling_symbol_table;
	} else { /* ZEND_OVERLOADED_FUNCTION */
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		INIT_ZVAL(*(EX_T(opline->result.u.var).var.ptr));

			/* Not sure what should be done here if it's a static method */
		if (EX(object)) {
			Z_OBJ_HT_P(EX(object))->call_method(EX(fbc)->common.function_name, opline->extended_value, EX_T(opline->result.u.var).var.ptr, EX(object), return_value_used TSRMLS_CC);
		} else {
			zend_error_noreturn(E_ERROR, "Cannot call overloaded function for non-object");
		}

		if (EX(function_state).function->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			efree(EX(function_state).function->common.function_name);
		}
		efree(EX(fbc));

		if (!return_value_used) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		} else {
			EX_T(opline->result.u.var).var.ptr->is_ref = 0;
			EX_T(opline->result.u.var).var.ptr->refcount = 1;
		}
	}

	if (EG(This)) {
		if (EG(exception) && EX(fbc) && EX(fbc)->common.fn_flags&ZEND_ACC_CTOR) {
			EG(This)->refcount--;
			if (EG(This)->refcount == 1) {
			    zend_object_store_ctor_failed(EG(This) TSRMLS_CC);
			}
			zval_ptr_dtor(&EG(This));
		} else if (should_change_scope) {
			zval_ptr_dtor(&EG(This));
		}
	}

	if (should_change_scope) {
		EG(This) = current_this;
		EG(scope) = current_scope;
	}
	zend_ptr_stack_3_pop(&EG(arg_types_stack), (void**)&EX(calling_scope), (void**)&EX(object), (void**)&EX(fbc));

	EX(function_state).function = (zend_function *) EX(op_array);
	EG(function_state_ptr) = &EX(function_state);
	zend_ptr_stack_clear_multiple(TSRMLS_C);

	if (EG(exception)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
		if (return_value_used && EX_T(opline->result.u.var).var.ptr) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_DO_FCALL_BY_NAME_SPEC() OPDEF(ZEND_DO_FCALL_BY_NAME, M_ANY, M_ANY)
#if HAVE_OP(ZEND_DO_FCALL_BY_NAME)
ZEND_VM_HANDLER(ZEND_DO_FCALL_BY_NAME)
{
	EX(function_state).function = EX(fbc);
	ZEND_VM_DISPATCH_TO_HELPER(zend_do_fcall_common_helper);
}
#endif

#define ZEND_DO_FCALL_SPEC() OPDEF(ZEND_DO_FCALL, M_CONST, M_ANY)
#if HAVE_OP(ZEND_DO_FCALL)
ZEND_VM_HANDLER(ZEND_DO_FCALL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *fname = GET_OP1_ZVAL_PTR(BP_VAR_R);

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(calling_scope));

	if (zend_hash_find(EG(function_table), fname->value.str.val, fname->value.str.len+1, (void **) &EX(function_state).function)==FAILURE) {
		zend_error_noreturn(E_ERROR, "Unknown function:  %s()\n", fname->value.str.val);
	}
	EX(object) = NULL;
	EX(calling_scope) = EX(function_state).function->common.scope;

	FREE_OP1();

	ZEND_VM_DISPATCH_TO_HELPER(zend_do_fcall_common_helper);
}
#endif

#define ZEND_RETURN_SPEC() OPDEF(ZEND_RETURN, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_RETURN)
ZEND_VM_HANDLER(ZEND_RETURN)
{
	zend_op *opline = EX(opline);
	zval *retval_ptr;
	zval **retval_ptr_ptr;
	zend_free_op free_op1;

	if (EG(active_op_array)->return_reference == ZEND_RETURN_REF) {

		if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_STRICT, "Only variable references should be returned by reference");
			ZEND_VM_C_GOTO(return_by_value);
		}

		retval_ptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

		if (!retval_ptr_ptr) {
			zend_error_noreturn(E_ERROR, "Cannot return string offsets by reference");
		}

		if (!(*retval_ptr_ptr)->is_ref) {
			if (EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr
				|| (opline->extended_value == ZEND_RETURNS_FUNCTION && !EX_T(opline->op1.u.var).var.fcall_returned_reference)) {
				zend_error(E_STRICT, "Only variable references should be returned by reference");
				PZVAL_LOCK(*retval_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
				ZEND_VM_C_GOTO(return_by_value);
			}
		}

		SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr_ptr);
		(*retval_ptr_ptr)->refcount++;

		(*EG(return_value_ptr_ptr)) = (*retval_ptr_ptr);
	} else {
ZEND_VM_C_LABEL(return_by_value):

		retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

		if (EG(ze1_compatibility_mode) && Z_TYPE_P(retval_ptr) == IS_OBJECT) {
			zval *ret;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			if (Z_OBJ_HT_P(retval_ptr)->clone_obj == NULL) {
				zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s",  Z_OBJCE_P(retval_ptr)->name);
			}
			zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", Z_OBJCE_P(retval_ptr)->name);
			ret->value.obj = Z_OBJ_HT_P(retval_ptr)->clone_obj(retval_ptr TSRMLS_CC);
			*EG(return_value_ptr_ptr) = ret;
		} else if (!IS_OP1_TMP_FREE()) { /* Not a temp var */
			if (PZVAL_IS_REF(retval_ptr) && retval_ptr->refcount > 0) {
				zval *ret;

				ALLOC_ZVAL(ret);
				INIT_PZVAL_COPY(ret, retval_ptr);
				zval_copy_ctor(ret);
				*EG(return_value_ptr_ptr) = ret;
			} else {
				*EG(return_value_ptr_ptr) = retval_ptr;
				retval_ptr->refcount++;
			}
		} else {
			zval *ret;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			*EG(return_value_ptr_ptr) = ret;
		}
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
}
#endif

#define ZEND_THROW_SPEC() OPDEF(ZEND_THROW, M_VAR, M_ANY)
#if HAVE_OP(ZEND_THROW)
ZEND_VM_HANDLER(ZEND_THROW)
{
	zend_op *opline = EX(opline);
	zval *value;
	zval *exception;
	zend_free_op free_op1;

	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (value->type != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "Can only throw objects");
	}
	/* Not sure if a complete copy is what we want here */
	ALLOC_ZVAL(exception);
	INIT_PZVAL_COPY(exception, value);
	if (!IS_OP1_TMP_FREE()) {
		zval_copy_ctor(exception);
	}

	zend_throw_exception_object(exception TSRMLS_CC);
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_CATCH_SPEC() OPDEF(ZEND_CATCH, M_ANY, M_ANY)
#if HAVE_OP(ZEND_CATCH)
ZEND_VM_HANDLER(ZEND_CATCH)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce;

	/* Check whether an exception has been thrown, if not, jump over code */
	if (EG(exception) == NULL) {
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
	}
	ce = Z_OBJCE_P(EG(exception));
	if (ce != EX_T(opline->op1.u.var).class_entry) {
		if (!instanceof_function(ce, EX_T(opline->op1.u.var).class_entry TSRMLS_CC)) {
			if (opline->op1.u.EA.type) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
				ZEND_VM_NEXT_OPCODE();
			}
			ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
			ZEND_VM_CONTINUE(); /* CHECK_ME */
		}
	}

	zend_hash_update(EG(active_symbol_table), opline->op2.u.constant.value.str.val,
		opline->op2.u.constant.value.str.len+1, &EG(exception), sizeof(zval *), (void **) NULL);
	EG(exception) = NULL;
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_SEND_VAL_SPEC() OPDEF(ZEND_SEND_VAL, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_SEND_VAL)
ZEND_VM_HANDLER(ZEND_SEND_VAL)
{
	zend_op *opline = EX(opline);
	if (opline->extended_value==ZEND_DO_FCALL_BY_NAME
		&& ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
			zend_error_noreturn(E_ERROR, "Cannot pass parameter %d by reference", opline->op2.u.opline_num);
	}
	{
		zval *valptr;
		zval *value;
		zend_free_op free_op1;

		value = GET_OP1_ZVAL_PTR(BP_VAR_R);

		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, value);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(valptr);
		}
		zend_ptr_stack_push(&EG(argument_stack), valptr);
		FREE_OP1_IF_VAR();
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#if OP1_OP2_MASK(M_VAR, M_ANY)
ZEND_VM_HELPER(zend_send_by_var_helper)
{
	zend_op *opline = EX(opline);
	zval *varptr;
	zend_free_op free_op1;
	varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (varptr == &EG(uninitialized_zval)) {
		ALLOC_ZVAL(varptr);
		INIT_ZVAL(*varptr);
		varptr->refcount = 0;
	} else if (PZVAL_IS_REF(varptr)) {
		zval *original_var = varptr;

		ALLOC_ZVAL(varptr);
		*varptr = *original_var;
		varptr->is_ref = 0;
		varptr->refcount = 0;
		zval_copy_ctor(varptr);
	}
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);
	FREE_OP1();  /* for string offsets */

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_SEND_VAR_NO_REF_SPEC() OPDEF(ZEND_SEND_VAR_NO_REF, M_VAR, M_ANY)
#if HAVE_OP(ZEND_SEND_VAR_NO_REF)
ZEND_VM_HANDLER(ZEND_SEND_VAR_NO_REF)
{
	zend_op *opline = EX(opline);
	if (opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) { /* Had function_ptr at compile_time */
		if (!(opline->extended_value & ZEND_ARG_SEND_BY_REF)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
		}
	} else if (!ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
	}
	{
		zval *varptr;
		zend_free_op free_op1;
		varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

		if (varptr != &EG(uninitialized_zval) && (PZVAL_IS_REF(varptr) || varptr->refcount == 1)) {
			varptr->is_ref = 1;
			varptr->refcount++;
			zend_ptr_stack_push(&EG(argument_stack), varptr);
			FREE_OP1_IF_VAR();
			ZEND_VM_NEXT_OPCODE();
		}
		zend_error_noreturn(E_ERROR, "Only variables can be passed by reference");
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_SEND_REF_SPEC() OPDEF(ZEND_SEND_REF, M_VAR, M_ANY)
#if HAVE_OP(ZEND_SEND_REF)
ZEND_VM_HANDLER_EX(ZEND_SEND_REF)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **varptr_ptr;
	zval *varptr;
	varptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

	if (!varptr_ptr) {
		zend_error_noreturn(E_ERROR, "Only variables can be passed by reference");
	}

	SEPARATE_ZVAL_TO_MAKE_IS_REF(varptr_ptr);
	varptr = *varptr_ptr;
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_SEND_VAR_SPEC() OPDEF(ZEND_SEND_VAR, M_VAR, M_ANY)
#if HAVE_OP(ZEND_SEND_VAR)
ZEND_VM_HANDLER(ZEND_SEND_VAR)
{
	zend_op *opline = EX(opline);

	if ((opline->extended_value == ZEND_DO_FCALL_BY_NAME)
		&& ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_REF);
	}
	ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
}
#endif

#define ZEND_RECV_SPEC() OPDEF(ZEND_RECV, M_ANY, M_ANY)
#if HAVE_OP(ZEND_RECV)
ZEND_VM_HANDLER(ZEND_RECV)
{
	zend_op *opline = EX(opline);
	zval **param;
	zend_uint arg_num = opline->op1.u.constant.value.lval;

	if (zend_ptr_stack_get_arg(arg_num, (void **) &param TSRMLS_CC)==FAILURE) {
		char *space;
		char *class_name = get_active_class_name(&space TSRMLS_CC);
		zend_execute_data *ptr = EX(prev_execute_data);

		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, NULL TSRMLS_CC);
		if(ptr && ptr->op_array) {
			zend_error(E_WARNING, "Missing argument %ld for %s%s%s(), called in %s on line %d and defined", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C), ptr->op_array->filename, ptr->opline->lineno);
		} else {
			zend_error(E_WARNING, "Missing argument %ld for %s%s%s()", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C));
		}
		if (opline->result.op_type == IS_VAR) {
			PZVAL_UNLOCK_FREE(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {
		zend_free_op free_res;
		zval **var_ptr;

		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, *param TSRMLS_CC);
		var_ptr = get_zval_ptr_ptr(&opline->result, EX(Ts), &free_res, BP_VAR_W);
		if (PZVAL_IS_REF(*param)) {
			zend_assign_to_variable_reference(var_ptr, param TSRMLS_CC);
		} else {
			zend_receive(var_ptr, *param TSRMLS_CC);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_RECV_INIT_SPEC() OPDEF(ZEND_RECV_INIT, M_ANY, M_CONST)
#if HAVE_OP(ZEND_RECV_INIT)
ZEND_VM_HANDLER(ZEND_RECV_INIT)
{
	zend_op *opline = EX(opline);
	zval **param, *assignment_value;
	zend_uint arg_num = opline->op1.u.constant.value.lval;
	zend_free_op free_res;
	zval **var_ptr = get_zval_ptr_ptr(&opline->result, EX(Ts), &free_res, BP_VAR_W);

	if (zend_ptr_stack_get_arg(arg_num, (void **) &param TSRMLS_CC)==FAILURE) {
		if (opline->op2.u.constant.type == IS_CONSTANT || opline->op2.u.constant.type==IS_CONSTANT_ARRAY) {
			zval *default_value;

			ALLOC_ZVAL(default_value);
			*default_value = opline->op2.u.constant;
			if (opline->op2.u.constant.type==IS_CONSTANT_ARRAY) {
				zval_copy_ctor(default_value);
			}
			default_value->refcount=1;
			zval_update_constant(&default_value, 0 TSRMLS_CC);
			default_value->refcount=0;
			default_value->is_ref=0;
			param = &default_value;
			assignment_value = default_value;
		} else {
			param = NULL;
			assignment_value = &opline->op2.u.constant;
		}
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value TSRMLS_CC);
		zend_receive(var_ptr, assignment_value TSRMLS_CC);
	} else {
		assignment_value = *param;
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value TSRMLS_CC);
		if (PZVAL_IS_REF(assignment_value)) {
			zend_assign_to_variable_reference(var_ptr, param TSRMLS_CC);
		} else {
			zend_receive(var_ptr, assignment_value TSRMLS_CC);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BOOL_SPEC() OPDEF(ZEND_BOOL, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_BOOL)
ZEND_VM_HANDLER(ZEND_BOOL)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
	EX_T(opline->result.u.var).tmp_var.value.lval = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));
	EX_T(opline->result.u.var).tmp_var.type = IS_BOOL;
	FREE_OP1();

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BRK_SPEC() OPDEF(ZEND_BRK, M_ANY, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_BRK)
ZEND_VM_HANDLER(ZEND_BRK)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	ZEND_VM_SET_OPCODE(EX(op_array)->opcodes +
	  zend_brk_cont(GET_OP2_ZVAL_PTR(BP_VAR_R),
	    opline->op1.u.opline_num,
	    EX(op_array), EX(Ts) TSRMLS_CC)->brk);
	FREE_OP2();
	ZEND_VM_CONTINUE(); /* CHECK_ME */
}
#endif

#define ZEND_CONT_SPEC() OPDEF(ZEND_CONT, M_ANY, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_CONT)
ZEND_VM_HANDLER(ZEND_CONT)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;

	ZEND_VM_SET_OPCODE(EX(op_array)->opcodes +
	  zend_brk_cont(GET_OP2_ZVAL_PTR(BP_VAR_R),
	    opline->op1.u.opline_num,
	    EX(op_array), EX(Ts) TSRMLS_CC)->cont);
	FREE_OP2();
	ZEND_VM_CONTINUE(); /* CHECK_ME */
}
#endif

#define ZEND_CASE_SPEC() OPDEF(ZEND_CASE, M_CONST_TMP_VAR, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_CASE)
ZEND_VM_HANDLER(ZEND_CASE)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1, free_op2;

	if (OP1_TYPE==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 GET_OP1_ZVAL_PTR(BP_VAR_R),
				 GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);

	FREE_OP2();
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		FREE_OP1();
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_SWITCH_FREE_SPEC() OPDEF(ZEND_SWITCH_FREE, M_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_SWITCH_FREE)
ZEND_VM_HANDLER(ZEND_SWITCH_FREE)
{
	zend_switch_free(EX(opline), EX(Ts) TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_NEW_SPEC() OPDEF(ZEND_NEW, M_ANY, M_ANY)
#if HAVE_OP(ZEND_NEW)
ZEND_VM_HANDLER(ZEND_NEW)
{
	zend_op *opline = EX(opline);

	if (EX_T(opline->op1.u.var).class_entry->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		char *class_type;

		if (EX_T(opline->op1.u.var).class_entry->ce_flags & ZEND_ACC_INTERFACE) {
			class_type = "interface";
		} else {
			class_type = "abstract class";
		}
		zend_error_noreturn(E_ERROR, "Cannot instantiate %s %s", class_type,  EX_T(opline->op1.u.var).class_entry->name);
	}
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
	object_init_ex(EX_T(opline->result.u.var).var.ptr, EX_T(opline->op1.u.var).class_entry);
	EX_T(opline->result.u.var).var.ptr->refcount=1;
	EX_T(opline->result.u.var).var.ptr->is_ref=0;

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_CLONE_SPEC() OPDEF(ZEND_CLONE, M_CONST_TMP_VAR_UNUSED, M_ANY)
#if HAVE_OP(ZEND_CLONE)
ZEND_VM_HANDLER(ZEND_CLONE)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *obj = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R);
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	if (!obj || Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error(E_WARNING, "__clone method called on non-object");
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
		FREE_OP1_IF_VAR();
		ZEND_VM_NEXT_OPCODE();
	}

	ce = Z_OBJCE_P(obj);
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (!clone_call) {
		zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name);
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (ce != EG(scope)) {
				zend_error_noreturn(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(clone->common.scope, EG(scope))) {
				zend_error_noreturn(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
	EX_T(opline->result.u.var).var.ptr->value.obj = clone_call(obj TSRMLS_CC);
	if (EG(exception)) {
		FREE_ZVAL(EX_T(opline->result.u.var).var.ptr);
	} else {
		EX_T(opline->result.u.var).var.ptr->type = IS_OBJECT;
		EX_T(opline->result.u.var).var.ptr->refcount=1;
		EX_T(opline->result.u.var).var.ptr->is_ref=1;
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FETCH_CONSTANT_SPEC() OPDEF(ZEND_FETCH_CONSTANT, M_CONST_UNUSED, M_CONST)
#if HAVE_OP(ZEND_FETCH_CONSTANT)
ZEND_VM_HANDLER(ZEND_FETCH_CONSTANT)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce = NULL;
	zval **value;

	if (OP1_TYPE == IS_UNUSED) {
/* This seems to be a reminant of namespaces
		if (EG(scope)) {
			ce = EG(scope);
			if (zend_hash_find(&ce->constants_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, (void **) &value) == SUCCESS) {
				zval_update_constant(value, (void *) 1 TSRMLS_CC);
				EX_T(opline->result.u.var).tmp_var = **value;
				zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
				ZEND_VM_NEXT_OPCODE();
			}
		}
*/
		if (!zend_get_constant(opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len, &EX_T(opline->result.u.var).tmp_var TSRMLS_CC)) {
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
						opline->op2.u.constant.value.str.val,
						opline->op2.u.constant.value.str.val);
			EX_T(opline->result.u.var).tmp_var = opline->op2.u.constant;
			zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	ce = EX_T(opline->op1.u.var).class_entry;

	if (zend_hash_find(&ce->constants_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, (void **) &value) == SUCCESS) {
		zval_update_constant(value, (void *) 1 TSRMLS_CC);
		EX_T(opline->result.u.var).tmp_var = **value;
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	} else {
		zend_error_noreturn(E_ERROR, "Undefined class constant '%s'", opline->op2.u.constant.value.str.val);
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#if OP1_OP2_MASK(M_CONST_TMP_VAR_UNUSED, M_CONST_TMP_VAR_UNUSED)
ZEND_VM_HELPER(zend_init_add_array_helper)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr, **expr_ptr_ptr = NULL;
	zval *offset=GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (opline->extended_value) {
		expr_ptr_ptr=GET_OP1_ZVAL_PTR_PTR(BP_VAR_R);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=GET_OP1_ZVAL_PTR(BP_VAR_R);
	}

	if (opline->opcode == ZEND_INIT_ARRAY) {
		array_init(array_ptr);
		if (!expr_ptr) {
			ZEND_VM_NEXT_OPCODE();
		}
	}
	if (!opline->extended_value && IS_OP1_TMP_FREE()) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			expr_ptr->refcount++;
		} else if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
		switch (offset->type) {
			case IS_DOUBLE:
				zend_hash_index_update(array_ptr->value.ht, (long) offset->value.dval, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(array_ptr->value.ht, offset->value.lval, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(array_ptr->value.ht, offset->value.str.val, offset->value.str.len+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(array_ptr->value.ht, "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		FREE_OP2();
	} else {
		zend_hash_next_index_insert(array_ptr->value.ht, &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {
		FREE_OP1_VAR_PTR();
	} else {
		FREE_OP1_IF_VAR();
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_INIT_ARRAY_SPEC() OPDEF(ZEND_INIT_ARRAY, M_CONST_TMP_VAR_UNUSED, M_CONST_TMP_VAR_UNUSED)
#if HAVE_OP(ZEND_INIT_ARRAY)
ZEND_VM_HANDLER(ZEND_INIT_ARRAY)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_init_add_array_helper);
}
#endif

#define ZEND_ADD_ARRAY_ELEMENT_SPEC() OPDEF(ZEND_ADD_ARRAY_ELEMENT, M_CONST_TMP_VAR_UNUSED, M_CONST_TMP_VAR_UNUSED)
#if HAVE_OP(ZEND_ADD_ARRAY_ELEMENT)
ZEND_VM_HANDLER(ZEND_ADD_ARRAY_ELEMENT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_init_add_array_helper);
}
#endif

#define ZEND_CAST_SPEC() OPDEF(ZEND_CAST, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_CAST)
ZEND_VM_HANDLER(ZEND_CAST)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *expr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	*result = *expr;
	if (!IS_OP1_TMP_FREE()) {
		zendi_zval_copy_ctor(*result);
	}
	switch (opline->extended_value) {
		case IS_NULL:
			convert_to_null(result);
			break;
		case IS_BOOL:
			convert_to_boolean(result);
			break;
		case IS_LONG:
			convert_to_long(result);
			break;
		case IS_DOUBLE:
			convert_to_double(result);
			break;
		case IS_STRING: {
			zval var_copy;
			int use_copy;

			zend_make_printable_zval(result, &var_copy, &use_copy);
			if (use_copy) {
				zval_dtor(result);
				*result = var_copy;
			}
			break;
		}
		case IS_ARRAY:
			convert_to_array(result);
			break;
		case IS_OBJECT:
			convert_to_object(result);
			break;
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_INCLUDE_OR_EVAL_SPEC() OPDEF(ZEND_INCLUDE_OR_EVAL, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_INCLUDE_OR_EVAL)
ZEND_VM_HANDLER(ZEND_INCLUDE_OR_EVAL)
{
	zend_op *opline = EX(opline);
	zend_op_array *new_op_array=NULL;
	zval **original_return_value = EG(return_value_ptr_ptr);
	int return_value_used;
	zend_free_op free_op1;
	zval *inc_filename = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	if (inc_filename->type!=IS_STRING) {
		tmp_inc_filename = *inc_filename;
		zval_copy_ctor(&tmp_inc_filename);
		convert_to_string(&tmp_inc_filename);
		inc_filename = &tmp_inc_filename;
	}

	return_value_used = RETURN_VALUE_USED(opline);

	switch (opline->op2.u.constant.value.lval) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				int dummy = 1;
				zend_file_handle file_handle;

				if (SUCCESS == zend_stream_open(inc_filename->value.str.val, &file_handle TSRMLS_CC)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = estrndup(inc_filename->value.str.val, inc_filename->value.str.len);
					}

					if (zend_hash_add(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1, (void *)&dummy, sizeof(int), NULL)==SUCCESS) {
						new_op_array = zend_compile_file(&file_handle, (opline->op2.u.constant.value.lval==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
						zend_destroy_file_handle(&file_handle TSRMLS_CC);
					} else {
						zend_file_handle_dtor(&file_handle);
						failure_retval=1;
					}
				} else {
					if (opline->op2.u.constant.value.lval==ZEND_INCLUDE_ONCE) {
						zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, inc_filename->value.str.val);
					} else {
						zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, inc_filename->value.str.val);
					}
				}
				break;
			}
			break;
		case ZEND_INCLUDE:
		case ZEND_REQUIRE:
			new_op_array = compile_filename(opline->op2.u.constant.value.lval, inc_filename TSRMLS_CC);
			break;
		case ZEND_EVAL: {
				char *eval_desc = zend_make_compiled_string_description("eval()'d code" TSRMLS_CC);

				new_op_array = compile_string(inc_filename, eval_desc TSRMLS_CC);
				efree(eval_desc);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	if (inc_filename==&tmp_inc_filename) {
		zval_dtor(&tmp_inc_filename);
	}
	FREE_OP1();
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (new_op_array) {
		zval *saved_object;
		zend_function *saved_function;

		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = new_op_array;
		EX_T(opline->result.u.var).var.ptr = NULL;

		saved_object = EX(object);
		saved_function = EX(function_state).function;

		EX(function_state).function = (zend_function *) new_op_array;
		EX(object) = NULL;

		zend_execute(new_op_array TSRMLS_CC);

		EX(function_state).function = saved_function;
		EX(object) = saved_object;

		if (!return_value_used) {
			if (EX_T(opline->result.u.var).var.ptr) {
				zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
			}
		} else { /* return value is used */
			if (!EX_T(opline->result.u.var).var.ptr) { /* there was no return statement */
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				EX_T(opline->result.u.var).var.ptr->value.lval = 1;
				EX_T(opline->result.u.var).var.ptr->type = IS_BOOL;
			}
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(function_state_ptr) = &EX(function_state);
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		if (EG(exception)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
		}
	} else {
		if (return_value_used) {
			ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
			INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			EX_T(opline->result.u.var).var.ptr->value.lval = failure_retval;
			EX_T(opline->result.u.var).var.ptr->type = IS_BOOL;
		}
	}
	EG(return_value_ptr_ptr) = original_return_value;
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_UNSET_VAR_SPEC() OPDEF(ZEND_UNSET_VAR, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_UNSET_VAR)
ZEND_VM_HANDLER(ZEND_UNSET_VAR)
{
	zend_op *opline = EX(opline);
	zval tmp, *varname;
	HashTable *target_symbol_table;
	zend_free_op free_op1;

	varname = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (varname->type != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		zend_std_unset_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		zend_hash_del(target_symbol_table, varname->value.str.val, varname->value.str.len+1);
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_UNSET_DIM_OBJ_SPEC() OPDEF(ZEND_UNSET_DIM_OBJ, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_UNSET_DIM_OBJ)
ZEND_VM_HANDLER(ZEND_UNSET_DIM_OBJ)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_R);
	zval *offset = GET_OP2_ZVAL_PTR(BP_VAR_R);
	long index;

	if (container) {
		HashTable *ht;

		if (opline->extended_value == ZEND_UNSET_DIM) {
			switch (Z_TYPE_PP(container)) {
				case IS_ARRAY:
					ht = Z_ARRVAL_PP(container);
					break;
				case IS_OBJECT:
					ht = NULL;
					if (!Z_OBJ_HT_P(*container)->unset_dimension) {
						zend_error_noreturn(E_ERROR, "Cannot use object as array");
					}
					Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
					break;
				case IS_STRING:
					zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
					ZEND_VM_CONTINUE(); /* bailed out before */
				default:
					ht = NULL;
					break;
			}
		} else { /* ZEND_UNSET_OBJ */
			ht = NULL;
			if (Z_TYPE_PP(container) == IS_OBJECT) {
				Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			}
		}
		if (ht)	{
			switch (offset->type) {
				case IS_DOUBLE:
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					if (offset->type == IS_DOUBLE) {
						index = (long) offset->value.dval;
					} else {
						index = offset->value.lval;
					}

					zend_hash_index_del(ht, index);
					break;
				case IS_STRING:
					zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1);
					break;
				case IS_NULL:
					zend_hash_del(ht, "", sizeof(""));
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in unset");
					break;
			}
		}
	} else {
		/* overloaded element */
	}
	FREE_OP2();
	FREE_OP1_VAR_PTR();

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FE_RESET_SPEC() OPDEF(ZEND_FE_RESET, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_FE_RESET)
ZEND_VM_HANDLER(ZEND_FE_RESET)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr, **array_ptr_ptr;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	zend_class_entry *ce = NULL;

	if (opline->extended_value) {
		array_ptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_R);
		if (array_ptr_ptr == NULL) {
			ALLOC_INIT_ZVAL(array_ptr);
		} else if (Z_TYPE_PP(array_ptr_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_PP(array_ptr_ptr);
			if (!ce || ce->get_iterator == NULL) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				(*array_ptr_ptr)->refcount++;
			}
			array_ptr = *array_ptr_ptr;
		} else {
			SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
			array_ptr = *array_ptr_ptr;
			array_ptr->refcount++;
		}
	} else {
		array_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (IS_OP1_TMP_FREE()) { /* IS_TMP_VAR */
			zval *tmp;

			ALLOC_ZVAL(tmp);
			INIT_PZVAL_COPY(tmp, array_ptr);
			array_ptr = tmp;
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_P(array_ptr);
		} else {
			array_ptr->refcount++;
		}
	}

	if (ce && ce->get_iterator) {
		iter = ce->get_iterator(ce, array_ptr TSRMLS_CC);

		if (iter) {
			array_ptr = zend_iterator_wrap(iter TSRMLS_CC);
		} else {
			array_ptr->refcount++;
		}
	}

	PZVAL_LOCK(array_ptr);
	EX_T(opline->result.u.var).var.ptr = array_ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
		}
	} else if ((fe_ht = HASH_OF(array_ptr)) != NULL) {
		/* probably redundant */
		zend_hash_internal_pointer_reset(fe_ht);
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");

		opline++;
		ZEND_VM_SET_OPCODE(EX(op_array)->opcodes+opline->op2.u.opline_num);
		if (opline->extended_value) {
			FREE_OP1_VAR_PTR();
		} else {
			FREE_OP1_IF_VAR();
		}
		ZEND_VM_CONTINUE_JMP();
	}

	if (opline->extended_value) {
		FREE_OP1_VAR_PTR();
	} else {
		FREE_OP1_IF_VAR();
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_FE_FETCH_SPEC() OPDEF(ZEND_FE_FETCH, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_FE_FETCH)
ZEND_VM_HANDLER(ZEND_FE_FETCH)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval **value, *key;
	char *str_key;
	uint str_key_len;
	ulong int_key;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	int key_type;
	zend_bool use_key = opline->extended_value & ZEND_FE_FETCH_WITH_KEY;

	PZVAL_LOCK(array);

	switch (zend_iterator_unwrap(array, &iter TSRMLS_CC)) {
		default:
		case ZEND_ITER_INVALID:
			zend_error(E_WARNING, "Invalid argument supplied for foreach()");
			ZEND_VM_SET_OPCODE(EX(op_array)->opcodes+opline->op2.u.opline_num);
			ZEND_VM_CONTINUE_JMP();

		case ZEND_ITER_PLAIN_OBJECT: {
			char *class_name, *prop_name;
			zend_object *zobj = zend_objects_get_address(array TSRMLS_CC);

			fe_ht = HASH_OF(array);
			do {
				if (zend_hash_get_current_data(fe_ht, (void **) &value)==FAILURE) {
					/* reached end of iteration */
					ZEND_VM_SET_OPCODE(EX(op_array)->opcodes+opline->op2.u.opline_num);
					ZEND_VM_CONTINUE_JMP();
				}
				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);

				zend_hash_move_forward(fe_ht);
			} while (key_type != HASH_KEY_IS_STRING || zend_check_property_access(zobj, str_key TSRMLS_CC) != SUCCESS);
			if (use_key) {
				zend_unmangle_property_name(str_key, &class_name, &prop_name);
				str_key_len = strlen(prop_name);
				str_key = estrndup(prop_name, str_key_len);
				str_key_len++;
			}
			break;
		}

		case ZEND_ITER_PLAIN_ARRAY:
			fe_ht = HASH_OF(array);
			if (zend_hash_get_current_data(fe_ht, (void **) &value)==FAILURE) {
				/* reached end of iteration */
				ZEND_VM_SET_OPCODE(EX(op_array)->opcodes+opline->op2.u.opline_num);
				ZEND_VM_CONTINUE_JMP();
			}
			if (use_key) {
				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 1, NULL);
			}
			zend_hash_move_forward(fe_ht);
			break;

		case ZEND_ITER_OBJECT:
			/* !iter happens from exception */
			if (iter && iter->index++) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter TSRMLS_CC);
			}
			if (!iter || iter->funcs->valid(iter TSRMLS_CC) == FAILURE) {
				/* reached end of iteration */
				ZEND_VM_SET_OPCODE(EX(op_array)->opcodes+opline->op2.u.opline_num);
				ZEND_VM_CONTINUE_JMP();
			}
			iter->funcs->get_current_data(iter, &value TSRMLS_CC);
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_SET_OPCODE(EX(op_array)->opcodes+opline->op2.u.opline_num);
				ZEND_VM_CONTINUE_JMP();
			}
			if (use_key) {
				if (iter->funcs->get_current_key) {
					key_type = iter->funcs->get_current_key(iter, &str_key, &str_key_len, &int_key TSRMLS_CC);
				} else {
					key_type = HASH_KEY_IS_LONG;
					int_key = iter->index;
				}
			}

			break;
	}

	if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
		SEPARATE_ZVAL_IF_NOT_REF(value);
		(*value)->is_ref = 1;
	}

	if (!use_key) {
		if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
			EX_T(opline->result.u.var).var.ptr_ptr = value;
			(*value)->refcount++;
		} else {
			zval *result = &EX_T(opline->result.u.var).tmp_var;

			*result = **value;
			zval_copy_ctor(result);
		}
	} else {
		zval *result = &EX_T(opline->result.u.var).tmp_var;

		(*value)->refcount++;

		array_init(result);

		zend_hash_index_update(result->value.ht, 0, value, sizeof(zval *), NULL);

		ALLOC_ZVAL(key);
		INIT_PZVAL(key);

		switch (key_type) {
			case HASH_KEY_IS_STRING:
				key->value.str.val = str_key;
				key->value.str.len = str_key_len-1;
				key->type = IS_STRING;
				break;
			case HASH_KEY_IS_LONG:
				key->value.lval = int_key;
				key->type = IS_LONG;
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
		zend_hash_index_update(result->value.ht, 1, &key, sizeof(zval *), NULL);
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_JMP_NO_CTOR_SPEC() OPDEF(ZEND_JMP_NO_CTOR, M_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_JMP_NO_CTOR)
ZEND_VM_HANDLER(ZEND_JMP_NO_CTOR)
{
	zend_op *opline = EX(opline);
	zval *object_zval;
	zend_function *constructor;
	zend_free_op free_op1;

	if (OP1_TYPE == IS_VAR) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}

	object_zval = GET_OP1_ZVAL_PTR(BP_VAR_R);
	constructor = Z_OBJ_HT_P(object_zval)->get_constructor(object_zval TSRMLS_CC);

	EX(fbc_constructor) = NULL;
	if (constructor == NULL) {
		if(opline->op1.u.EA.type & EXT_TYPE_UNUSED) {
			zval_ptr_dtor(EX_T(opline->op1.u.var).var.ptr_ptr);
		}
		ZEND_VM_SET_OPCODE(EX(op_array)->opcodes + opline->op2.u.opline_num);
		ZEND_VM_CONTINUE_JMP();
	} else {
		EX(fbc_constructor) = constructor;
	}

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ISSET_ISEMPTY_VAR_SPEC() OPDEF(ZEND_ISSET_ISEMPTY_VAR, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_ISSET_ISEMPTY_VAR)
ZEND_VM_HANDLER(ZEND_ISSET_ISEMPTY_VAR)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval tmp, *varname = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval **value;
	zend_bool isset = 1;
	HashTable *target_symbol_table;

	if (varname->type != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		value = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 1 TSRMLS_CC);
		if (!value) {
			isset = 0;
		}
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &value) == FAILURE) {
			isset = 0;
		}
	}

	EX_T(opline->result.u.var).tmp_var.type = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			if (isset && Z_TYPE_PP(value) == IS_NULL) {
				EX_T(opline->result.u.var).tmp_var.value.lval = 0;
			} else {
				EX_T(opline->result.u.var).tmp_var.value.lval = isset;
			}
			break;
		case ZEND_ISEMPTY:
			if (!isset || !i_zend_is_true(*value)) {
				EX_T(opline->result.u.var).tmp_var.value.lval = 1;
			} else {
				EX_T(opline->result.u.var).tmp_var.value.lval = 0;
			}
			break;
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}
	FREE_OP1();

	ZEND_VM_NEXT_OPCODE();
}
#endif

#if OP1_OP2_MASK(M_VAR_UNUSED, M_CONST_TMP_VAR)
ZEND_VM_HELPER_EX(zend_isset_isempty_dim_prop_obj_handler, int prop_dim)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_R);
	zval *offset = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		if ((*container)->type == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = (*container)->value.ht;

			switch (offset->type) {
				case IS_DOUBLE:
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					if (offset->type == IS_DOUBLE) {
						index = (long) offset->value.dval;
					} else {
						index = offset->value.lval;
					}
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in unset");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
		} else if ((*container)->type == IS_OBJECT) {
			if (prop_dim) {
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			}
		} else if ((*container)->type == IS_STRING) { /* string offsets */
			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (offset->value.lval < Z_STRLEN_PP(container)) {
						result = 1;
					}
					break;
				case ZEND_ISEMPTY:
					if (offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
						result = 1;
					}
					break;
			}
		}
	}

	EX_T(opline->result.u.var).tmp_var.type = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			EX_T(opline->result.u.var).tmp_var.value.lval = result;
			break;
		case ZEND_ISEMPTY:
			EX_T(opline->result.u.var).tmp_var.value.lval = !result;
			break;
	}

	FREE_OP2();
	FREE_OP1_VAR_PTR();

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ISSET_ISEMPTY_DIM_OBJ_SPEC() OPDEF(ZEND_ISSET_ISEMPTY_DIM_OBJ, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ISSET_ISEMPTY_DIM_OBJ)
ZEND_VM_HANDLER(ZEND_ISSET_ISEMPTY_DIM_OBJ)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_isset_isempty_dim_prop_obj_handler, prop_dim, 0);
}
#endif

#define ZEND_ISSET_ISEMPTY_PROP_OBJ_SPEC() OPDEF(ZEND_ISSET_ISEMPTY_PROP_OBJ, M_VAR_UNUSED, M_CONST_TMP_VAR)
#if HAVE_OP(ZEND_ISSET_ISEMPTY_PROP_OBJ)
ZEND_VM_HANDLER(ZEND_ISSET_ISEMPTY_PROP_OBJ)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_isset_isempty_dim_prop_obj_handler, prop_dim, 1);
}
#endif

#define ZEND_EXIT_SPEC() OPDEF(ZEND_EXIT, M_CONST_TMP_VAR_UNUSED, M_ANY)
#if HAVE_OP(ZEND_EXIT)
ZEND_VM_HANDLER(ZEND_EXIT)
{
	zend_op *opline = EX(opline);

	if (OP1_TYPE != IS_UNUSED) {
		zval *ptr;
		zend_free_op free_op1;

		ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (Z_TYPE_P(ptr) == IS_LONG) {
			EG(exit_status) = Z_LVAL_P(ptr);
		} else {
			zend_print_variable(ptr);
		}
		FREE_OP1();
	}
	zend_bailout();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_BEGIN_SILENCE_SPEC() OPDEF(ZEND_BEGIN_SILENCE, M_ANY, M_ANY)
#if HAVE_OP(ZEND_BEGIN_SILENCE)
ZEND_VM_HANDLER(ZEND_BEGIN_SILENCE)
{
	zend_op *opline = EX(opline);

	EX_T(opline->result.u.var).tmp_var.value.lval = EG(error_reporting);
	EX_T(opline->result.u.var).tmp_var.type = IS_LONG;  /* shouldn't be necessary */
	zend_alter_ini_entry("error_reporting", sizeof("error_reporting"), "0", 1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_RAISE_ABSTRACT_ERROR_SPEC() OPDEF(ZEND_RAISE_ABSTRACT_ERROR, M_ANY, M_ANY)
#if HAVE_OP(ZEND_RAISE_ABSTRACT_ERROR)
ZEND_VM_HANDLER(ZEND_RAISE_ABSTRACT_ERROR)
{
	zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EG(scope)->name, EX(op_array)->function_name);
	ZEND_VM_NEXT_OPCODE(); /* Never reached */
}
#endif

#define ZEND_END_SILENCE_SPEC() OPDEF(ZEND_END_SILENCE, M_TMP, M_ANY)
#if HAVE_OP(ZEND_END_SILENCE)
ZEND_VM_HANDLER(ZEND_END_SILENCE)
{
	zend_op *opline = EX(opline);
	zval restored_error_reporting;

	if (!EG(error_reporting)) {
		restored_error_reporting.type = IS_LONG;
		restored_error_reporting.value.lval = EX_T(opline->op1.u.var).tmp_var.value.lval;
		convert_to_string(&restored_error_reporting);
		zend_alter_ini_entry("error_reporting", sizeof("error_reporting"), Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
		zendi_zval_dtor(restored_error_reporting);
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_QM_ASSIGN_SPEC() OPDEF(ZEND_QM_ASSIGN, M_CONST_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_QM_ASSIGN)
ZEND_VM_HANDLER(ZEND_QM_ASSIGN)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	EX_T(opline->result.u.var).tmp_var = *value;
	if (!IS_OP1_TMP_FREE()) {
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_EXT_STMT_SPEC() OPDEF(ZEND_EXT_STMT, M_ANY, M_ANY)
#if HAVE_OP(ZEND_EXT_STMT)
ZEND_VM_HANDLER(ZEND_EXT_STMT)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_statement_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_EXT_FCALL_BEGIN_SPEC() OPDEF(ZEND_EXT_FCALL_BEGIN, M_ANY, M_ANY)
#if HAVE_OP(ZEND_EXT_FCALL_BEGIN)
ZEND_VM_HANDLER(ZEND_EXT_FCALL_BEGIN)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_begin_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_EXT_FCALL_END_SPEC() OPDEF(ZEND_EXT_FCALL_END, M_ANY, M_ANY)
#if HAVE_OP(ZEND_EXT_FCALL_END)
ZEND_VM_HANDLER(ZEND_EXT_FCALL_END)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_end_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_DECLARE_CLASS_SPEC() OPDEF(ZEND_DECLARE_CLASS, M_ANY, M_ANY)
#if HAVE_OP(ZEND_DECLARE_CLASS)
ZEND_VM_HANDLER(ZEND_DECLARE_CLASS)
{
	zend_op *opline = EX(opline);

	EX_T(opline->result.u.var).class_entry = do_bind_class(opline, EG(class_table), 0 TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_DECLARE_INHERITED_CLASS_SPEC() OPDEF(ZEND_DECLARE_INHERITED_CLASS, M_ANY, M_ANY)
#if HAVE_OP(ZEND_DECLARE_INHERITED_CLASS)
ZEND_VM_HANDLER(ZEND_DECLARE_INHERITED_CLASS)
{
	zend_op *opline = EX(opline);

	EX_T(opline->result.u.var).class_entry = do_bind_inherited_class(opline, EG(class_table), EX_T(opline->extended_value).class_entry, 0 TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_DECLARE_FUNCTION_SPEC() OPDEF(ZEND_DECLARE_FUNCTION, M_ANY, M_ANY)
#if HAVE_OP(ZEND_DECLARE_FUNCTION)
ZEND_VM_HANDLER(ZEND_DECLARE_FUNCTION)
{
	do_bind_function(EX(opline), EG(function_table), 0);
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_TICKS_SPEC() OPDEF(ZEND_TICKS, M_CONST, M_ANY)
#if HAVE_OP(ZEND_TICKS)
ZEND_VM_HANDLER(ZEND_TICKS)
{
	zend_op *opline = EX(opline);

	if (++EG(ticks_count)>=opline->op1.u.constant.value.lval) {
		EG(ticks_count)=0;
		if (zend_ticks_function) {
			zend_ticks_function(opline->op1.u.constant.value.lval);
		}
	}
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_INSTANCEOF_SPEC() OPDEF(ZEND_INSTANCEOF, M_TMP_VAR, M_ANY)
#if HAVE_OP(ZEND_INSTANCEOF)
ZEND_VM_HANDLER(ZEND_INSTANCEOF)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *expr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zend_bool result;

	if (Z_TYPE_P(expr) == IS_OBJECT) {
		result = instanceof_function(Z_OBJCE_P(expr), EX_T(opline->op2.u.var).class_entry TSRMLS_CC);
	} else {
		result = 0;
	}
	ZVAL_BOOL(&EX_T(opline->result.u.var).tmp_var, result);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_EXT_NOP_SPEC() OPDEF(ZEND_EXT_NOP, M_ANY, M_ANY)
#if HAVE_OP(ZEND_EXT_NOP)
ZEND_VM_HANDLER(ZEND_EXT_NOP)
{
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_NOP_SPEC() OPDEF(ZEND_NOP, M_ANY, M_ANY)
#if HAVE_OP(ZEND_NOP)
ZEND_VM_HANDLER(ZEND_NOP)
{
	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_ADD_INTERFACE_SPEC() OPDEF(ZEND_ADD_INTERFACE, M_ANY, M_ANY)
#if HAVE_OP(ZEND_ADD_INTERFACE)
ZEND_VM_HANDLER(ZEND_ADD_INTERFACE)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce = EX_T(opline->op1.u.var).class_entry;
	zend_class_entry *iface = EX_T(opline->op2.u.var).class_entry;

	if (!(iface->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_error_noreturn(E_ERROR, "%s cannot implement %s - it is not an interface", ce->name, iface->name);
	}

	ce->interfaces[opline->extended_value] = iface;

	zend_do_implement_interface(ce, iface TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}
#endif

#define ZEND_HANDLE_EXCEPTION_SPEC() OPDEF(ZEND_HANDLE_EXCEPTION, M_ANY, M_ANY)
#if HAVE_OP(ZEND_HANDLE_EXCEPTION)
ZEND_VM_HANDLER(ZEND_HANDLE_EXCEPTION)
{
	zend_uint op_num = EG(opline_before_exception)-EG(active_op_array)->opcodes;
	int i;
	int encapsulating_block=-1;
	zval **stack_zval_pp;

	stack_zval_pp = (zval **) EG(argument_stack).top_element - 1;
	while (*stack_zval_pp != NULL) {
		zval_ptr_dtor(stack_zval_pp);
		EG(argument_stack).top_element--;
		stack_zval_pp--;
	}

	for (i=0; i<EG(active_op_array)->last_try_catch; i++) {
		if (EG(active_op_array)->try_catch_array[i].try_op > op_num) {
			/* further blocks will not be relevant... */
			break;
		}
		if (op_num >= EG(active_op_array)->try_catch_array[i].try_op
			&& op_num < EG(active_op_array)->try_catch_array[i].catch_op) {
			encapsulating_block = i;
		}
	}

	if (encapsulating_block == -1) {
		ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
	} else {
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[EG(active_op_array)->try_catch_array[encapsulating_block].catch_op]);
		ZEND_VM_CONTINUE();
	}
}
#endif

#define ZEND_VERIFY_ABSTRACT_CLASS_SPEC() OPDEF(ZEND_VERIFY_ABSTRACT_CLASS, M_ANY, M_ANY)
#if HAVE_OP(ZEND_VERIFY_ABSTRACT_CLASS)
ZEND_VM_HANDLER(ZEND_VERIFY_ABSTRACT_CLASS)
{
	zend_verify_abstract_class(EX_T(EX(opline)->op1.u.var).class_entry TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}
#endif
