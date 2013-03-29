#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO

/* ops that use CLs:
op1:
ZEND_FETCH_CONSTANT:
ZEND_INIT_CTOR_CALL:
ZEND_INIT_STATIC_METHOD_CALL:
ZEND_INIT_METHOD_CALL:
ZEND_IMPORT_CLASS:
ZEND_IMPORT_FUNCTION:
ZEND_IMPORT_CONST:
ZEND_ADD_INTERFACE:
ZEND_VERIFY_ABSTRACT_CLASS:
ZEND_NEW:
ZEND_CATCH:
ZEND_INIT_FCALL_BY_NAME:

op2:
ZEND_UNSET_VAR:
ZEND_ISSET_ISEMPTY_VAR:
ZEND_FETCH_UNSET:
ZEND_FETCH_IS:
ZEND_FETCH_R:
ZEND_FETCH_W:
ZEND_FETCH_RW:
ZEND_FETCH_FUNC_ARG:
ZEND_ADD_INTERFACE:
ZEND_INSTANCEOF:

extended_value:
ZEND_DECLARE_INHERITED_CLASS:

ignore result
INIT_METHOD_CALL:
*/

#define OP1_CONST_IS_CLASS 1
#define OP2_CONST_IS_CLASS 2
#define EXT_CONST_IS_CLASS 4
#define RESULT_IS_UNUSED   8

static const char op_const_means_class[256]  = {
	/* 0 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 32 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	/* 64 */
	0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2,
	/* 96 */
	0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 9, 1, 2, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 128 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 160 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 192 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 224 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

#define GET_AVAILABLE_T()		\
	for (i = 0; i < T; i++) {	\
		if (!taken_T[i]) {		\
			break;				\
		}						\
	}							\
	taken_T[i] = 1;				\
	if (i > max) {				\
		max = i;				\
	}

static void optimize_temporary_variables(zend_op_array *op_array)
{
	int T = op_array->T;
	char *taken_T;			/* T index in use */
	zend_op **start_of_T;	/* opline where T is first used */
	char *valid_T;			/* Is the map_T valid */
	int *map_T;				/* Map's the T to its new index */
	zend_op *opline, *end;
	int currT;
	int i;
	int max = -1;
	int var_to_free = -1;

	taken_T = (char *) emalloc(T);
	start_of_T = (zend_op **) emalloc(T * sizeof(zend_op *));
	valid_T = (char *) emalloc(T);
	map_T = (int *) emalloc(T * sizeof(int));

    end = op_array->opcodes;
    opline = &op_array->opcodes[op_array->last - 1];

    /* Find T definition points */
    while (opline >= end) {
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
        if (ZEND_RESULT_TYPE(opline) & (IS_VAR | IS_TMP_VAR | IS_CONST)) {
			if (!(op_const_means_class[opline->opcode] & RESULT_IS_UNUSED)) {
				start_of_T[VAR_NUM(ZEND_RESULT(opline).var)] = opline;
			}
		}
#else
        if (ZEND_RESULT_TYPE(opline) & (IS_VAR | IS_TMP_VAR)) {
			start_of_T[VAR_NUM(ZEND_RESULT(opline).var)] = opline;
		}
#endif
		opline--;
	}

	memset(valid_T, 0, T);
	memset(taken_T, 0, T);

    end = op_array->opcodes;
    opline = &op_array->opcodes[op_array->last - 1];

    while (opline >= end) {
		if ((ZEND_OP1_TYPE(opline) & (IS_VAR | IS_TMP_VAR))
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
			|| ((op_const_means_class[opline->opcode] & OP1_CONST_IS_CLASS) && ZEND_OP1_TYPE(opline) == IS_CONST)
#endif
			) {
			currT = VAR_NUM(ZEND_OP1(opline).var);
			if (!valid_T[currT]) {
				GET_AVAILABLE_T();
				map_T[currT] = i;
				valid_T[currT] = 1;
			}
			ZEND_OP1(opline).var = NUM_VAR(map_T[currT]);
		}

		/* Skip OP_DATA */
		if (opline->opcode == ZEND_OP_DATA &&
		    (opline-1)->opcode == ZEND_ASSIGN_DIM) {
		    opline--;
		    continue;
		}

		if ((ZEND_OP2_TYPE(opline) & (IS_VAR | IS_TMP_VAR))
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
			|| ((op_const_means_class[opline->opcode] & OP2_CONST_IS_CLASS) && ZEND_OP2_TYPE(opline) == IS_CONST)
#endif
		   ) {
			currT = VAR_NUM(ZEND_OP2(opline).var);
			if (!valid_T[currT]) {
				GET_AVAILABLE_T();
				map_T[currT] = i;
				valid_T[currT] = 1;
			}
			ZEND_OP2(opline).var = NUM_VAR(map_T[currT]);
		}

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
		if ((op_const_means_class[opline->opcode] & EXT_CONST_IS_CLASS)) {
#else
		if (opline->opcode == ZEND_DECLARE_INHERITED_CLASS ||
            opline->opcode == ZEND_DECLARE_INHERITED_CLASS_DELAYED) {
#endif
			currT = VAR_NUM(opline->extended_value);
			if (!valid_T[currT]) {
				GET_AVAILABLE_T();
				map_T[currT] = i;
				valid_T[currT] = 1;
			}
			opline->extended_value = NUM_VAR(map_T[currT]);
		}

		/* Allocate OP_DATA->op2 after "operands", but before "result" */
		if (opline->opcode == ZEND_ASSIGN_DIM &&
		    (opline + 1)->opcode == ZEND_OP_DATA &&
		    ZEND_OP2_TYPE(opline + 1) & (IS_VAR | IS_TMP_VAR)) {
			currT = VAR_NUM(ZEND_OP2(opline + 1).var);
			GET_AVAILABLE_T();
			map_T[currT] = i;
			valid_T[currT] = 1;
			taken_T[i] = 0;
			ZEND_OP2(opline + 1).var = NUM_VAR(i);
			var_to_free = i;
		}

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
		if (ZEND_RESULT_TYPE(opline) & (IS_VAR | IS_TMP_VAR | IS_CONST)) {
			if (!(op_const_means_class[opline->opcode] & RESULT_IS_UNUSED)) {
#else
		if (ZEND_RESULT_TYPE(opline) & (IS_VAR | IS_TMP_VAR)) {
#endif
				currT = VAR_NUM(ZEND_RESULT(opline).var);
				if (valid_T[currT]) {
					if (start_of_T[currT] == opline) {
						taken_T[map_T[currT]] = 0;
					}
					ZEND_RESULT(opline).var = NUM_VAR(map_T[currT]);
				} else { /* Au still needs to be assigned a T which is a bit dumb. Should consider changing Zend */
					GET_AVAILABLE_T();

					if (RESULT_UNUSED(opline)) {
						taken_T[i] = 0;
					} else {
						/* Code which gets here is using a wrongly built opcode such as RECV() */
						map_T[currT] = i;
						valid_T[currT] = 1;
					}
					ZEND_RESULT(opline).var = NUM_VAR(i);
				}
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
			}
#endif
		}

		if (var_to_free >= 0) {
			taken_T[var_to_free] = 0;
			var_to_free = -1;
		}

		opline--;
	}

	efree(taken_T);
	efree(start_of_T);
	efree(valid_T);
	efree(map_T);
	op_array->T = max + 1;
}
