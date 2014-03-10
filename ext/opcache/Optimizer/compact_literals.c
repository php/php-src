/* pass 11
 * - compact literals table
 */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO

#define DEBUG_COMPACT_LITERALS 0

#define LITERAL_VALUE                        0x0100
#define LITERAL_FUNC                         0x0200
#define LITERAL_CLASS                        0x0300
#define LITERAL_CONST                        0x0400
#define LITERAL_CLASS_CONST                  0x0500
#define LITERAL_STATIC_METHOD                0x0600
#define LITERAL_STATIC_PROPERTY              0x0700
#define LITERAL_METHOD                       0x0800
#define LITERAL_PROPERTY                     0x0900

#define LITERAL_EX_CLASS                     0x4000
#define LITERAL_EX_OBJ                       0x2000
#define LITERAL_MAY_MERGE                    0x1000
#define LITERAL_KIND_MASK                    0x0f00
#define LITERAL_NUM_RELATED_MASK             0x000f
#define LITERAL_NUM_SLOTS_MASK               0x00f0
#define LITERAL_NUM_SLOTS_SHIFT              4

#define LITERAL_NUM_RELATED(info) (info & LITERAL_NUM_RELATED_MASK)
#define LITERAL_NUM_SLOTS(info)   ((info & LITERAL_NUM_SLOTS_MASK) >> LITERAL_NUM_SLOTS_SHIFT)

typedef struct _literal_info {
	zend_uint  flags; /* bitmask (see defines above) */
	union {
		int    num;   /* variable number or class name literal number */
	} u;
} literal_info;

#define LITERAL_FLAGS(kind, slots, related) \
	((kind) | ((slots) << LITERAL_NUM_SLOTS_SHIFT) | (related))

#define LITERAL_INFO(n, kind, merge, slots, related) do { \
		info[n].flags = (((merge) ? LITERAL_MAY_MERGE : 0) | LITERAL_FLAGS(kind, slots, related)); \
	} while (0)

#define LITERAL_INFO_CLASS(n, kind, merge, slots, related, _num) do { \
		info[n].flags = (LITERAL_EX_CLASS | ((merge) ? LITERAL_MAY_MERGE : 0) | LITERAL_FLAGS(kind, slots, related)); \
		info[n].u.num = (_num); \
	} while (0)

#define LITERAL_INFO_OBJ(n, kind, merge, slots, related, _num) do { \
		info[n].flags = (LITERAL_EX_OBJ | ((merge) ? LITERAL_MAY_MERGE : 0) | LITERAL_FLAGS(kind, slots, related)); \
		info[n].u.num = (_num); \
	} while (0)

static void optimizer_literal_obj_info(literal_info   *info,
                                       zend_uchar      op_type,
                                       znode_op        op,
                                       int             constant,
                                       zend_uint       kind,
                                       zend_uint       slots,
                                       zend_uint       related,
                                       zend_op_array  *op_array)
{
	/* For now we merge only $this object properties and methods.
	 * In general it's also possible to do it for any CV variable as well,
	 * but it would require complex dataflow and/or type analysis.
	 */
	if (Z_TYPE(op_array->literals[constant].constant) == IS_STRING &&
	    op_type == IS_UNUSED) {
		LITERAL_INFO_OBJ(constant, kind, 1, slots, related, op_array->this_var);
	} else {
		LITERAL_INFO(constant, kind, 0, slots, related);
	}
}

static void optimizer_literal_class_info(literal_info   *info,
                                         zend_uchar      op_type,
                                         znode_op        op,
                                         int             constant,
                                         zend_uint       kind,
                                         zend_uint       slots,
                                         zend_uint       related,
                                         zend_op_array  *op_array)
{
	if (op_type == IS_CONST) {
		LITERAL_INFO_CLASS(constant, kind, 1, slots, related, op.constant);
	} else {
		LITERAL_INFO(constant, kind, 0, slots, related);
	}
}

static void optimizer_compact_literals(zend_op_array *op_array TSRMLS_DC)
{
	zend_op *opline, *end;
	int i, j, n, *pos, *map, cache_slots;
	ulong h;
	literal_info *info;
	int l_null = -1;
	int l_false = -1;
	int l_true = -1;
	HashTable hash;
	char *key;
	int key_len;

	if (op_array->last_literal) {
		info = (literal_info*)ecalloc(op_array->last_literal, sizeof(literal_info));

	    /* Mark literals of specific types */
		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
			switch (opline->opcode) {
				case ZEND_DO_FCALL:
					LITERAL_INFO(opline->op1.constant, LITERAL_FUNC, 1, 1, 1);
					break;
				case ZEND_INIT_FCALL_BY_NAME:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, LITERAL_FUNC, 1, 1, 2);
					}
					break;
				case ZEND_INIT_NS_FCALL_BY_NAME:
					LITERAL_INFO(opline->op2.constant, LITERAL_FUNC, 1, 1, 3);
					break;
				case ZEND_INIT_METHOD_CALL:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						optimizer_literal_obj_info(
							info,
							opline->op1_type,
							opline->op1,
							opline->op2.constant,
							LITERAL_METHOD, 2, 2,
							op_array);
					}
					break;
				case ZEND_INIT_STATIC_METHOD_CALL:
					if (ZEND_OP1_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, LITERAL_CLASS, 1, 1, 2);
					}
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						optimizer_literal_class_info(
							info,
							opline->op1_type,
							opline->op1,
							opline->op2.constant,
							LITERAL_STATIC_METHOD, (ZEND_OP1_TYPE(opline) == IS_CONST) ? 1 : 2, 2,
							op_array);
					}
					break;
				case ZEND_CATCH:
					LITERAL_INFO(opline->op1.constant, LITERAL_CLASS, 1, 1, 2);
					break;
				case ZEND_FETCH_CONSTANT:
					if (ZEND_OP1_TYPE(opline) == IS_UNUSED) {
						if ((opline->extended_value & (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) == (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) {
							LITERAL_INFO(opline->op2.constant, LITERAL_CONST, 1, 1, 5);
						} else {
							LITERAL_INFO(opline->op2.constant, LITERAL_CONST, 1, 1, 3);
						}
					} else {
						if (ZEND_OP1_TYPE(opline) == IS_CONST) {
							LITERAL_INFO(opline->op1.constant, LITERAL_CLASS, 1, 1, 2);
						}
						optimizer_literal_class_info(
							info,
							opline->op1_type,
							opline->op1,
							opline->op2.constant,
							LITERAL_CLASS_CONST, (ZEND_OP1_TYPE(opline) == IS_CONST) ? 1 : 2, 1,
							op_array);
					}
					break;
				case ZEND_FETCH_R:
				case ZEND_FETCH_W:
				case ZEND_FETCH_RW:
				case ZEND_FETCH_IS:
				case ZEND_FETCH_UNSET:
				case ZEND_FETCH_FUNC_ARG:
				case ZEND_UNSET_VAR:
				case ZEND_ISSET_ISEMPTY_VAR:
					if (ZEND_OP2_TYPE(opline) == IS_UNUSED) {
						if (ZEND_OP1_TYPE(opline) == IS_CONST) {
							LITERAL_INFO(opline->op1.constant, LITERAL_VALUE, 1, 0, 1);
						}
					} else {
						if (ZEND_OP2_TYPE(opline) == IS_CONST) {
							LITERAL_INFO(opline->op2.constant, LITERAL_CLASS, 1, 1, 2);
						}
						if (ZEND_OP1_TYPE(opline) == IS_CONST) {
							optimizer_literal_class_info(
								info,
								opline->op2_type,
								opline->op2,
								opline->op1.constant,
								LITERAL_STATIC_PROPERTY, 2, 1,
								op_array);
						}
					}
					break;
				case ZEND_FETCH_CLASS:
				case ZEND_ADD_INTERFACE:
				case ZEND_ADD_TRAIT:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, LITERAL_CLASS, 1, 1, 2);
					}
					break;
				case ZEND_ASSIGN_OBJ:
				case ZEND_FETCH_OBJ_R:
				case ZEND_FETCH_OBJ_W:
				case ZEND_FETCH_OBJ_RW:
				case ZEND_FETCH_OBJ_IS:
				case ZEND_FETCH_OBJ_UNSET:
				case ZEND_FETCH_OBJ_FUNC_ARG:
				case ZEND_UNSET_OBJ:
				case ZEND_PRE_INC_OBJ:
				case ZEND_PRE_DEC_OBJ:
				case ZEND_POST_INC_OBJ:
				case ZEND_POST_DEC_OBJ:
				case ZEND_ISSET_ISEMPTY_PROP_OBJ:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						optimizer_literal_obj_info(
							info,
							opline->op1_type,
							opline->op1,
							opline->op2.constant,
							LITERAL_PROPERTY, 2, 1,
							op_array);
					}
					break;
				case ZEND_ASSIGN_ADD:
				case ZEND_ASSIGN_SUB:
				case ZEND_ASSIGN_MUL:
				case ZEND_ASSIGN_DIV:
				case ZEND_ASSIGN_MOD:
				case ZEND_ASSIGN_SL:
				case ZEND_ASSIGN_SR:
				case ZEND_ASSIGN_CONCAT:
				case ZEND_ASSIGN_BW_OR:
				case ZEND_ASSIGN_BW_AND:
				case ZEND_ASSIGN_BW_XOR:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						if (opline->extended_value == ZEND_ASSIGN_OBJ) {
							optimizer_literal_obj_info(
								info,
								opline->op1_type,
								opline->op1,
								opline->op2.constant,
								LITERAL_PROPERTY, 2, 1,
								op_array);
						} else {
							LITERAL_INFO(opline->op2.constant, LITERAL_VALUE, 1, 0, 1);
						}
					}
					break;
				default:
					if (ZEND_OP1_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, LITERAL_VALUE, 1, 0, 1);
					}
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, LITERAL_VALUE, 1, 0, 1);
					}
					break;
			}
			opline++;
		}

#if DEBUG_COMPACT_LITERALS
		{
			int i, use_copy;
			fprintf(stderr, "File %s func %s\n", op_array->filename,
					op_array->function_name? op_array->function_name : "main");
			fprintf(stderr, "Literlas table size %d\n", op_array->last_literal);

			for (i = 0; i < op_array->last_literal; i++) {
				zval zv = op_array->literals[i].constant;
				zend_make_printable_zval(&op_array->literals[i].constant, &zv, &use_copy);
				fprintf(stderr, "Literal %d, val (%d):%s\n", i, Z_STRLEN(zv), Z_STRVAL(zv));
				if (use_copy) {
					zval_dtor(&zv);
				}
			}
			fflush(stderr);
		}
#endif

		/* Merge equal constants */
		j = 0; cache_slots = 0;
		zend_hash_init(&hash, 16, NULL, NULL, 0);
		map = (int*)ecalloc(op_array->last_literal, sizeof(int));
		for (i = 0; i < op_array->last_literal; i++) {
			if (!info[i].flags) {
				/* unsed literal */
				zval_dtor(&op_array->literals[i].constant);
				continue;
			}
			switch (Z_TYPE(op_array->literals[i].constant)) {
				case IS_NULL:
					if (l_null < 0) {
						l_null = j;
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
					}
					map[i] = l_null;
					break;
				case IS_BOOL:
					if (Z_LVAL(op_array->literals[i].constant)) {
						if (l_true < 0) {
							l_true = j;
							if (i != j) {
								op_array->literals[j] = op_array->literals[i];
								info[j] = info[i];
							}
							j++;
						}
						map[i] = l_true;
					} else {
						if (l_false < 0) {
							l_false = j;
							if (i != j) {
								op_array->literals[j] = op_array->literals[i];
								info[j] = info[i];
							}
							j++;
						}
						map[i] = l_false;
					}
					break;
				case IS_LONG:
					if (zend_hash_index_find(&hash, Z_LVAL(op_array->literals[i].constant), (void**)&pos) == SUCCESS) {
						map[i] = *pos;
					} else {
						map[i] = j;
						zend_hash_index_update(&hash, Z_LVAL(op_array->literals[i].constant), (void**)&j, sizeof(int), NULL);
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
					}
					break;
				case IS_DOUBLE:
					if (zend_hash_find(&hash, (char*)&Z_DVAL(op_array->literals[i].constant), sizeof(double), (void**)&pos) == SUCCESS) {
						map[i] = *pos;
					} else {
						map[i] = j;
						zend_hash_add(&hash, (char*)&Z_DVAL(op_array->literals[i].constant), sizeof(double), (void**)&j, sizeof(int), NULL);
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
					}
					break;
				case IS_STRING:
				case IS_CONSTANT:
					if (info[i].flags & LITERAL_MAY_MERGE) {
						if (info[i].flags & LITERAL_EX_OBJ) {
							key_len = MAX_LENGTH_OF_LONG + sizeof("->") + Z_STRLEN(op_array->literals[i].constant);
							key = emalloc(key_len);
							key_len = snprintf(key, key_len-1, "%d->%s", info[i].u.num, Z_STRVAL(op_array->literals[i].constant));
						} else if (info[i].flags & LITERAL_EX_CLASS) {
							zval *class_name = &op_array->literals[(info[i].u.num < i) ? map[info[i].u.num] : info[i].u.num].constant;
							key_len = Z_STRLEN_P(class_name) + sizeof("::") + Z_STRLEN(op_array->literals[i].constant);
							key = emalloc(key_len);
							memcpy(key, Z_STRVAL_P(class_name), Z_STRLEN_P(class_name));
							memcpy(key + Z_STRLEN_P(class_name), "::", sizeof("::") - 1);
							memcpy(key + Z_STRLEN_P(class_name) + sizeof("::") - 1,
								Z_STRVAL(op_array->literals[i].constant),
								Z_STRLEN(op_array->literals[i].constant) + 1);
						} else {
							key = Z_STRVAL(op_array->literals[i].constant);
							key_len = Z_STRLEN(op_array->literals[i].constant)+1;
						}
						h = zend_hash_func(key, key_len);
						h += info[i].flags;
					}
					if ((info[i].flags & LITERAL_MAY_MERGE) &&
						zend_hash_quick_find(&hash, key, key_len, h, (void**)&pos) == SUCCESS &&
					   	Z_TYPE(op_array->literals[i].constant) == Z_TYPE(op_array->literals[*pos].constant) &&
						info[i].flags == info[*pos].flags) {

						if (info[i].flags & (LITERAL_EX_OBJ|LITERAL_EX_CLASS)) {
							efree(key);
						}
						map[i] = *pos;
						zval_dtor(&op_array->literals[i].constant);
						n = LITERAL_NUM_RELATED(info[i].flags);
						while (n > 1) {
							i++;
							zval_dtor(&op_array->literals[i].constant);
							n--;
						}
					} else {
						map[i] = j;
						if (info[i].flags & LITERAL_MAY_MERGE) {
							zend_hash_quick_add(&hash, key, key_len, h, (void**)&j, sizeof(int), NULL);
							if (info[i].flags & (LITERAL_EX_OBJ|LITERAL_EX_CLASS)) {
								efree(key);
							}
						}
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						if (!op_array->literals[j].hash_value) {
							if (IS_INTERNED(Z_STRVAL(op_array->literals[j].constant))) {
								op_array->literals[j].hash_value = INTERNED_HASH(Z_STRVAL(op_array->literals[j].constant));
							} else {
								op_array->literals[j].hash_value = zend_hash_func(Z_STRVAL(op_array->literals[j].constant), Z_STRLEN(op_array->literals[j].constant)+1);
							}
						}
						if (LITERAL_NUM_SLOTS(info[i].flags)) {
							op_array->literals[j].cache_slot = cache_slots;
							cache_slots += LITERAL_NUM_SLOTS(info[i].flags);
						}
						j++;
						n = LITERAL_NUM_RELATED(info[i].flags);
						while (n > 1) {
							i++;
							if (i != j) op_array->literals[j] = op_array->literals[i];
							if (!op_array->literals[j].hash_value) {
								if (IS_INTERNED(Z_STRVAL(op_array->literals[j].constant))) {
									op_array->literals[j].hash_value = INTERNED_HASH(Z_STRVAL(op_array->literals[j].constant));
								} else {
									op_array->literals[j].hash_value = zend_hash_func(Z_STRVAL(op_array->literals[j].constant), Z_STRLEN(op_array->literals[j].constant)+1);
								}
							}
							j++;
							n--;
						}
					}
					break;
				default:
					/* don't merge other types */
					map[i] = j;
					if (i != j) {
						op_array->literals[j] = op_array->literals[i];
						info[j] = info[i];
					}
					j++;
					break;
			}
		}
		zend_hash_destroy(&hash);
		op_array->last_literal = j;
		op_array->last_cache_slot = cache_slots;

	    /* Update opcodes to use new literals table */
		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
			if (ZEND_OP1_TYPE(opline) == IS_CONST) {
				opline->op1.constant = map[opline->op1.constant];
			}
			if (ZEND_OP2_TYPE(opline) == IS_CONST) {
				opline->op2.constant = map[opline->op2.constant];
			}
			opline++;
		}
		efree(map);
		efree(info);

#if DEBUG_COMPACT_LITERALS
		{
			int i, use_copy;
			fprintf(stderr, "Optimized literlas table size %d\n", op_array->last_literal);

			for (i = 0; i < op_array->last_literal; i++) {
				zval zv = op_array->literals[i].constant;
				zend_make_printable_zval(&op_array->literals[i].constant, &zv, &use_copy);
				fprintf(stderr, "Literal %d, val (%d):%s\n", i, Z_STRLEN(zv), Z_STRVAL(zv));
				if (use_copy) {
					zval_dtor(&zv);
				}
			}
			fflush(stderr);
		}
#endif
	}
}
#endif
