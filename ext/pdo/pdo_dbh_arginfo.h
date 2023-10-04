/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 98dfe635699876a6f017a46b49b1b56e12bb144c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dsn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, username, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_beginTransaction, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_commit arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_errorCode, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_errorInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_PDO_exec, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, statement, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_getAttribute, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_getAvailableDrivers arginfo_class_PDO_errorInfo

#define arginfo_class_PDO_inTransaction arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_PDO_lastInsertId, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_PDO_prepare, 0, 1, PDOStatement, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_PDO_query, 0, 1, PDOStatement, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fetchMode, IS_LONG, 1, "null")
	ZEND_ARG_VARIADIC_TYPE_INFO(0, fetchModeArgs, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_PDO_quote, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "PDO::PARAM_STR")
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_rollBack arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_setAttribute, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(PDO, __construct);
ZEND_METHOD(PDO, beginTransaction);
ZEND_METHOD(PDO, commit);
ZEND_METHOD(PDO, errorCode);
ZEND_METHOD(PDO, errorInfo);
ZEND_METHOD(PDO, exec);
ZEND_METHOD(PDO, getAttribute);
ZEND_METHOD(PDO, getAvailableDrivers);
ZEND_METHOD(PDO, inTransaction);
ZEND_METHOD(PDO, lastInsertId);
ZEND_METHOD(PDO, prepare);
ZEND_METHOD(PDO, query);
ZEND_METHOD(PDO, quote);
ZEND_METHOD(PDO, rollBack);
ZEND_METHOD(PDO, setAttribute);


static const zend_function_entry class_PDO_methods[] = {
	ZEND_ME(PDO, __construct, arginfo_class_PDO___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, beginTransaction, arginfo_class_PDO_beginTransaction, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, commit, arginfo_class_PDO_commit, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, errorCode, arginfo_class_PDO_errorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, errorInfo, arginfo_class_PDO_errorInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, exec, arginfo_class_PDO_exec, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, getAttribute, arginfo_class_PDO_getAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, getAvailableDrivers, arginfo_class_PDO_getAvailableDrivers, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(PDO, inTransaction, arginfo_class_PDO_inTransaction, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, lastInsertId, arginfo_class_PDO_lastInsertId, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, prepare, arginfo_class_PDO_prepare, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, query, arginfo_class_PDO_query, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, quote, arginfo_class_PDO_quote, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, rollBack, arginfo_class_PDO_rollBack, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, setAttribute, arginfo_class_PDO_setAttribute, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_PDO(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDO", class_PDO_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval const_PARAM_NULL_value;
	ZVAL_LONG(&const_PARAM_NULL_value, LONG_CONST(PDO_PARAM_NULL));
	zend_string *const_PARAM_NULL_name = zend_string_init_interned("PARAM_NULL", sizeof("PARAM_NULL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_NULL_name, &const_PARAM_NULL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_NULL_name);
	ZEND_ASSERT(LONG_CONST(PDO_PARAM_NULL) == 0);

	zval const_PARAM_BOOL_value;
	ZVAL_LONG(&const_PARAM_BOOL_value, LONG_CONST(PDO_PARAM_BOOL));
	zend_string *const_PARAM_BOOL_name = zend_string_init_interned("PARAM_BOOL", sizeof("PARAM_BOOL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_BOOL_name, &const_PARAM_BOOL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_BOOL_name);
	ZEND_ASSERT(LONG_CONST(PDO_PARAM_BOOL) == 5);

	zval const_PARAM_INT_value;
	ZVAL_LONG(&const_PARAM_INT_value, LONG_CONST(PDO_PARAM_INT));
	zend_string *const_PARAM_INT_name = zend_string_init_interned("PARAM_INT", sizeof("PARAM_INT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_INT_name, &const_PARAM_INT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_INT_name);
	ZEND_ASSERT(LONG_CONST(PDO_PARAM_INT) == 1);

	zval const_PARAM_STR_value;
	ZVAL_LONG(&const_PARAM_STR_value, LONG_CONST(PDO_PARAM_STR));
	zend_string *const_PARAM_STR_name = zend_string_init_interned("PARAM_STR", sizeof("PARAM_STR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_STR_name, &const_PARAM_STR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_STR_name);
	ZEND_ASSERT(LONG_CONST(PDO_PARAM_STR) == 2);

	zval const_PARAM_LOB_value;
	ZVAL_LONG(&const_PARAM_LOB_value, LONG_CONST(PDO_PARAM_LOB));
	zend_string *const_PARAM_LOB_name = zend_string_init_interned("PARAM_LOB", sizeof("PARAM_LOB") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_LOB_name, &const_PARAM_LOB_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_LOB_name);
	ZEND_ASSERT(LONG_CONST(PDO_PARAM_LOB) == 3);

	zval const_PARAM_STMT_value;
	ZVAL_LONG(&const_PARAM_STMT_value, LONG_CONST(PDO_PARAM_STMT));
	zend_string *const_PARAM_STMT_name = zend_string_init_interned("PARAM_STMT", sizeof("PARAM_STMT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_STMT_name, &const_PARAM_STMT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_STMT_name);
	ZEND_ASSERT(LONG_CONST(PDO_PARAM_STMT) == 4);

	zval const_PARAM_INPUT_OUTPUT_value;
	ZVAL_LONG(&const_PARAM_INPUT_OUTPUT_value, LONG_CONST(PDO_PARAM_INPUT_OUTPUT));
	zend_string *const_PARAM_INPUT_OUTPUT_name = zend_string_init_interned("PARAM_INPUT_OUTPUT", sizeof("PARAM_INPUT_OUTPUT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_INPUT_OUTPUT_name, &const_PARAM_INPUT_OUTPUT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_INPUT_OUTPUT_name);

	zval const_PARAM_STR_NATL_value;
	ZVAL_LONG(&const_PARAM_STR_NATL_value, LONG_CONST(PDO_PARAM_STR_NATL));
	zend_string *const_PARAM_STR_NATL_name = zend_string_init_interned("PARAM_STR_NATL", sizeof("PARAM_STR_NATL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_STR_NATL_name, &const_PARAM_STR_NATL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_STR_NATL_name);

	zval const_PARAM_STR_CHAR_value;
	ZVAL_LONG(&const_PARAM_STR_CHAR_value, LONG_CONST(PDO_PARAM_STR_CHAR));
	zend_string *const_PARAM_STR_CHAR_name = zend_string_init_interned("PARAM_STR_CHAR", sizeof("PARAM_STR_CHAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_STR_CHAR_name, &const_PARAM_STR_CHAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_STR_CHAR_name);

	zval const_PARAM_EVT_ALLOC_value;
	ZVAL_LONG(&const_PARAM_EVT_ALLOC_value, LONG_CONST(PDO_PARAM_EVT_ALLOC));
	zend_string *const_PARAM_EVT_ALLOC_name = zend_string_init_interned("PARAM_EVT_ALLOC", sizeof("PARAM_EVT_ALLOC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_EVT_ALLOC_name, &const_PARAM_EVT_ALLOC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_EVT_ALLOC_name);

	zval const_PARAM_EVT_FREE_value;
	ZVAL_LONG(&const_PARAM_EVT_FREE_value, LONG_CONST(PDO_PARAM_EVT_FREE));
	zend_string *const_PARAM_EVT_FREE_name = zend_string_init_interned("PARAM_EVT_FREE", sizeof("PARAM_EVT_FREE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_EVT_FREE_name, &const_PARAM_EVT_FREE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_EVT_FREE_name);

	zval const_PARAM_EVT_EXEC_PRE_value;
	ZVAL_LONG(&const_PARAM_EVT_EXEC_PRE_value, LONG_CONST(PDO_PARAM_EVT_EXEC_PRE));
	zend_string *const_PARAM_EVT_EXEC_PRE_name = zend_string_init_interned("PARAM_EVT_EXEC_PRE", sizeof("PARAM_EVT_EXEC_PRE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_EVT_EXEC_PRE_name, &const_PARAM_EVT_EXEC_PRE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_EVT_EXEC_PRE_name);

	zval const_PARAM_EVT_EXEC_POST_value;
	ZVAL_LONG(&const_PARAM_EVT_EXEC_POST_value, LONG_CONST(PDO_PARAM_EVT_EXEC_POST));
	zend_string *const_PARAM_EVT_EXEC_POST_name = zend_string_init_interned("PARAM_EVT_EXEC_POST", sizeof("PARAM_EVT_EXEC_POST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_EVT_EXEC_POST_name, &const_PARAM_EVT_EXEC_POST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_EVT_EXEC_POST_name);

	zval const_PARAM_EVT_FETCH_PRE_value;
	ZVAL_LONG(&const_PARAM_EVT_FETCH_PRE_value, LONG_CONST(PDO_PARAM_EVT_FETCH_PRE));
	zend_string *const_PARAM_EVT_FETCH_PRE_name = zend_string_init_interned("PARAM_EVT_FETCH_PRE", sizeof("PARAM_EVT_FETCH_PRE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_EVT_FETCH_PRE_name, &const_PARAM_EVT_FETCH_PRE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_EVT_FETCH_PRE_name);

	zval const_PARAM_EVT_FETCH_POST_value;
	ZVAL_LONG(&const_PARAM_EVT_FETCH_POST_value, LONG_CONST(PDO_PARAM_EVT_FETCH_POST));
	zend_string *const_PARAM_EVT_FETCH_POST_name = zend_string_init_interned("PARAM_EVT_FETCH_POST", sizeof("PARAM_EVT_FETCH_POST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_EVT_FETCH_POST_name, &const_PARAM_EVT_FETCH_POST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_EVT_FETCH_POST_name);

	zval const_PARAM_EVT_NORMALIZE_value;
	ZVAL_LONG(&const_PARAM_EVT_NORMALIZE_value, LONG_CONST(PDO_PARAM_EVT_NORMALIZE));
	zend_string *const_PARAM_EVT_NORMALIZE_name = zend_string_init_interned("PARAM_EVT_NORMALIZE", sizeof("PARAM_EVT_NORMALIZE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PARAM_EVT_NORMALIZE_name, &const_PARAM_EVT_NORMALIZE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PARAM_EVT_NORMALIZE_name);

	zval const_FETCH_DEFAULT_value;
	ZVAL_LONG(&const_FETCH_DEFAULT_value, LONG_CONST(PDO_FETCH_USE_DEFAULT));
	zend_string *const_FETCH_DEFAULT_name = zend_string_init_interned("FETCH_DEFAULT", sizeof("FETCH_DEFAULT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_DEFAULT_name, &const_FETCH_DEFAULT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_DEFAULT_name);

	zval const_FETCH_LAZY_value;
	ZVAL_LONG(&const_FETCH_LAZY_value, LONG_CONST(PDO_FETCH_LAZY));
	zend_string *const_FETCH_LAZY_name = zend_string_init_interned("FETCH_LAZY", sizeof("FETCH_LAZY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_LAZY_name, &const_FETCH_LAZY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_LAZY_name);

	zval const_FETCH_ASSOC_value;
	ZVAL_LONG(&const_FETCH_ASSOC_value, LONG_CONST(PDO_FETCH_ASSOC));
	zend_string *const_FETCH_ASSOC_name = zend_string_init_interned("FETCH_ASSOC", sizeof("FETCH_ASSOC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_ASSOC_name, &const_FETCH_ASSOC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_ASSOC_name);

	zval const_FETCH_NUM_value;
	ZVAL_LONG(&const_FETCH_NUM_value, LONG_CONST(PDO_FETCH_NUM));
	zend_string *const_FETCH_NUM_name = zend_string_init_interned("FETCH_NUM", sizeof("FETCH_NUM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_NUM_name, &const_FETCH_NUM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_NUM_name);

	zval const_FETCH_BOTH_value;
	ZVAL_LONG(&const_FETCH_BOTH_value, LONG_CONST(PDO_FETCH_BOTH));
	zend_string *const_FETCH_BOTH_name = zend_string_init_interned("FETCH_BOTH", sizeof("FETCH_BOTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_BOTH_name, &const_FETCH_BOTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_BOTH_name);

	zval const_FETCH_OBJ_value;
	ZVAL_LONG(&const_FETCH_OBJ_value, LONG_CONST(PDO_FETCH_OBJ));
	zend_string *const_FETCH_OBJ_name = zend_string_init_interned("FETCH_OBJ", sizeof("FETCH_OBJ") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_OBJ_name, &const_FETCH_OBJ_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_OBJ_name);

	zval const_FETCH_BOUND_value;
	ZVAL_LONG(&const_FETCH_BOUND_value, LONG_CONST(PDO_FETCH_BOUND));
	zend_string *const_FETCH_BOUND_name = zend_string_init_interned("FETCH_BOUND", sizeof("FETCH_BOUND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_BOUND_name, &const_FETCH_BOUND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_BOUND_name);

	zval const_FETCH_COLUMN_value;
	ZVAL_LONG(&const_FETCH_COLUMN_value, LONG_CONST(PDO_FETCH_COLUMN));
	zend_string *const_FETCH_COLUMN_name = zend_string_init_interned("FETCH_COLUMN", sizeof("FETCH_COLUMN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_COLUMN_name, &const_FETCH_COLUMN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_COLUMN_name);

	zval const_FETCH_CLASS_value;
	ZVAL_LONG(&const_FETCH_CLASS_value, LONG_CONST(PDO_FETCH_CLASS));
	zend_string *const_FETCH_CLASS_name = zend_string_init_interned("FETCH_CLASS", sizeof("FETCH_CLASS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_CLASS_name, &const_FETCH_CLASS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_CLASS_name);

	zval const_FETCH_INTO_value;
	ZVAL_LONG(&const_FETCH_INTO_value, LONG_CONST(PDO_FETCH_INTO));
	zend_string *const_FETCH_INTO_name = zend_string_init_interned("FETCH_INTO", sizeof("FETCH_INTO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_INTO_name, &const_FETCH_INTO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_INTO_name);

	zval const_FETCH_FUNC_value;
	ZVAL_LONG(&const_FETCH_FUNC_value, LONG_CONST(PDO_FETCH_FUNC));
	zend_string *const_FETCH_FUNC_name = zend_string_init_interned("FETCH_FUNC", sizeof("FETCH_FUNC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_FUNC_name, &const_FETCH_FUNC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_FUNC_name);

	zval const_FETCH_GROUP_value;
	ZVAL_LONG(&const_FETCH_GROUP_value, LONG_CONST(PDO_FETCH_GROUP));
	zend_string *const_FETCH_GROUP_name = zend_string_init_interned("FETCH_GROUP", sizeof("FETCH_GROUP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_GROUP_name, &const_FETCH_GROUP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_GROUP_name);

	zval const_FETCH_UNIQUE_value;
	ZVAL_LONG(&const_FETCH_UNIQUE_value, LONG_CONST(PDO_FETCH_UNIQUE));
	zend_string *const_FETCH_UNIQUE_name = zend_string_init_interned("FETCH_UNIQUE", sizeof("FETCH_UNIQUE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_UNIQUE_name, &const_FETCH_UNIQUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_UNIQUE_name);

	zval const_FETCH_KEY_PAIR_value;
	ZVAL_LONG(&const_FETCH_KEY_PAIR_value, LONG_CONST(PDO_FETCH_KEY_PAIR));
	zend_string *const_FETCH_KEY_PAIR_name = zend_string_init_interned("FETCH_KEY_PAIR", sizeof("FETCH_KEY_PAIR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_KEY_PAIR_name, &const_FETCH_KEY_PAIR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_KEY_PAIR_name);

	zval const_FETCH_CLASSTYPE_value;
	ZVAL_LONG(&const_FETCH_CLASSTYPE_value, LONG_CONST(PDO_FETCH_CLASSTYPE));
	zend_string *const_FETCH_CLASSTYPE_name = zend_string_init_interned("FETCH_CLASSTYPE", sizeof("FETCH_CLASSTYPE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_CLASSTYPE_name, &const_FETCH_CLASSTYPE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_CLASSTYPE_name);

	zval const_FETCH_SERIALIZE_value;
	ZVAL_LONG(&const_FETCH_SERIALIZE_value, LONG_CONST(PDO_FETCH_SERIALIZE));
	zend_string *const_FETCH_SERIALIZE_name = zend_string_init_interned("FETCH_SERIALIZE", sizeof("FETCH_SERIALIZE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_SERIALIZE_name, &const_FETCH_SERIALIZE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_SERIALIZE_name);

	zval const_FETCH_PROPS_LATE_value;
	ZVAL_LONG(&const_FETCH_PROPS_LATE_value, LONG_CONST(PDO_FETCH_PROPS_LATE));
	zend_string *const_FETCH_PROPS_LATE_name = zend_string_init_interned("FETCH_PROPS_LATE", sizeof("FETCH_PROPS_LATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_PROPS_LATE_name, &const_FETCH_PROPS_LATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_PROPS_LATE_name);

	zval const_FETCH_NAMED_value;
	ZVAL_LONG(&const_FETCH_NAMED_value, LONG_CONST(PDO_FETCH_NAMED));
	zend_string *const_FETCH_NAMED_name = zend_string_init_interned("FETCH_NAMED", sizeof("FETCH_NAMED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_NAMED_name, &const_FETCH_NAMED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_NAMED_name);

	zval const_ATTR_AUTOCOMMIT_value;
	ZVAL_LONG(&const_ATTR_AUTOCOMMIT_value, LONG_CONST(PDO_ATTR_AUTOCOMMIT));
	zend_string *const_ATTR_AUTOCOMMIT_name = zend_string_init_interned("ATTR_AUTOCOMMIT", sizeof("ATTR_AUTOCOMMIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_AUTOCOMMIT_name, &const_ATTR_AUTOCOMMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_AUTOCOMMIT_name);

	zval const_ATTR_PREFETCH_value;
	ZVAL_LONG(&const_ATTR_PREFETCH_value, LONG_CONST(PDO_ATTR_PREFETCH));
	zend_string *const_ATTR_PREFETCH_name = zend_string_init_interned("ATTR_PREFETCH", sizeof("ATTR_PREFETCH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_PREFETCH_name, &const_ATTR_PREFETCH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_PREFETCH_name);

	zval const_ATTR_TIMEOUT_value;
	ZVAL_LONG(&const_ATTR_TIMEOUT_value, LONG_CONST(PDO_ATTR_TIMEOUT));
	zend_string *const_ATTR_TIMEOUT_name = zend_string_init_interned("ATTR_TIMEOUT", sizeof("ATTR_TIMEOUT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_TIMEOUT_name, &const_ATTR_TIMEOUT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_TIMEOUT_name);

	zval const_ATTR_ERRMODE_value;
	ZVAL_LONG(&const_ATTR_ERRMODE_value, LONG_CONST(PDO_ATTR_ERRMODE));
	zend_string *const_ATTR_ERRMODE_name = zend_string_init_interned("ATTR_ERRMODE", sizeof("ATTR_ERRMODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_ERRMODE_name, &const_ATTR_ERRMODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_ERRMODE_name);

	zval const_ATTR_SERVER_VERSION_value;
	ZVAL_LONG(&const_ATTR_SERVER_VERSION_value, LONG_CONST(PDO_ATTR_SERVER_VERSION));
	zend_string *const_ATTR_SERVER_VERSION_name = zend_string_init_interned("ATTR_SERVER_VERSION", sizeof("ATTR_SERVER_VERSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SERVER_VERSION_name, &const_ATTR_SERVER_VERSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SERVER_VERSION_name);

	zval const_ATTR_CLIENT_VERSION_value;
	ZVAL_LONG(&const_ATTR_CLIENT_VERSION_value, LONG_CONST(PDO_ATTR_CLIENT_VERSION));
	zend_string *const_ATTR_CLIENT_VERSION_name = zend_string_init_interned("ATTR_CLIENT_VERSION", sizeof("ATTR_CLIENT_VERSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_CLIENT_VERSION_name, &const_ATTR_CLIENT_VERSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_CLIENT_VERSION_name);

	zval const_ATTR_SERVER_INFO_value;
	ZVAL_LONG(&const_ATTR_SERVER_INFO_value, LONG_CONST(PDO_ATTR_SERVER_INFO));
	zend_string *const_ATTR_SERVER_INFO_name = zend_string_init_interned("ATTR_SERVER_INFO", sizeof("ATTR_SERVER_INFO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SERVER_INFO_name, &const_ATTR_SERVER_INFO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SERVER_INFO_name);

	zval const_ATTR_CONNECTION_STATUS_value;
	ZVAL_LONG(&const_ATTR_CONNECTION_STATUS_value, LONG_CONST(PDO_ATTR_CONNECTION_STATUS));
	zend_string *const_ATTR_CONNECTION_STATUS_name = zend_string_init_interned("ATTR_CONNECTION_STATUS", sizeof("ATTR_CONNECTION_STATUS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_CONNECTION_STATUS_name, &const_ATTR_CONNECTION_STATUS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_CONNECTION_STATUS_name);

	zval const_ATTR_CASE_value;
	ZVAL_LONG(&const_ATTR_CASE_value, LONG_CONST(PDO_ATTR_CASE));
	zend_string *const_ATTR_CASE_name = zend_string_init_interned("ATTR_CASE", sizeof("ATTR_CASE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_CASE_name, &const_ATTR_CASE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_CASE_name);

	zval const_ATTR_CURSOR_NAME_value;
	ZVAL_LONG(&const_ATTR_CURSOR_NAME_value, LONG_CONST(PDO_ATTR_CURSOR_NAME));
	zend_string *const_ATTR_CURSOR_NAME_name = zend_string_init_interned("ATTR_CURSOR_NAME", sizeof("ATTR_CURSOR_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_CURSOR_NAME_name, &const_ATTR_CURSOR_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_CURSOR_NAME_name);

	zval const_ATTR_CURSOR_value;
	ZVAL_LONG(&const_ATTR_CURSOR_value, LONG_CONST(PDO_ATTR_CURSOR));
	zend_string *const_ATTR_CURSOR_name = zend_string_init_interned("ATTR_CURSOR", sizeof("ATTR_CURSOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_CURSOR_name, &const_ATTR_CURSOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_CURSOR_name);

	zval const_ATTR_ORACLE_NULLS_value;
	ZVAL_LONG(&const_ATTR_ORACLE_NULLS_value, LONG_CONST(PDO_ATTR_ORACLE_NULLS));
	zend_string *const_ATTR_ORACLE_NULLS_name = zend_string_init_interned("ATTR_ORACLE_NULLS", sizeof("ATTR_ORACLE_NULLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_ORACLE_NULLS_name, &const_ATTR_ORACLE_NULLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_ORACLE_NULLS_name);

	zval const_ATTR_PERSISTENT_value;
	ZVAL_LONG(&const_ATTR_PERSISTENT_value, LONG_CONST(PDO_ATTR_PERSISTENT));
	zend_string *const_ATTR_PERSISTENT_name = zend_string_init_interned("ATTR_PERSISTENT", sizeof("ATTR_PERSISTENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_PERSISTENT_name, &const_ATTR_PERSISTENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_PERSISTENT_name);

	zval const_ATTR_STATEMENT_CLASS_value;
	ZVAL_LONG(&const_ATTR_STATEMENT_CLASS_value, LONG_CONST(PDO_ATTR_STATEMENT_CLASS));
	zend_string *const_ATTR_STATEMENT_CLASS_name = zend_string_init_interned("ATTR_STATEMENT_CLASS", sizeof("ATTR_STATEMENT_CLASS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_STATEMENT_CLASS_name, &const_ATTR_STATEMENT_CLASS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_STATEMENT_CLASS_name);

	zval const_ATTR_FETCH_TABLE_NAMES_value;
	ZVAL_LONG(&const_ATTR_FETCH_TABLE_NAMES_value, LONG_CONST(PDO_ATTR_FETCH_TABLE_NAMES));
	zend_string *const_ATTR_FETCH_TABLE_NAMES_name = zend_string_init_interned("ATTR_FETCH_TABLE_NAMES", sizeof("ATTR_FETCH_TABLE_NAMES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_FETCH_TABLE_NAMES_name, &const_ATTR_FETCH_TABLE_NAMES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_FETCH_TABLE_NAMES_name);

	zval const_ATTR_FETCH_CATALOG_NAMES_value;
	ZVAL_LONG(&const_ATTR_FETCH_CATALOG_NAMES_value, LONG_CONST(PDO_ATTR_FETCH_CATALOG_NAMES));
	zend_string *const_ATTR_FETCH_CATALOG_NAMES_name = zend_string_init_interned("ATTR_FETCH_CATALOG_NAMES", sizeof("ATTR_FETCH_CATALOG_NAMES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_FETCH_CATALOG_NAMES_name, &const_ATTR_FETCH_CATALOG_NAMES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_FETCH_CATALOG_NAMES_name);

	zval const_ATTR_DRIVER_NAME_value;
	ZVAL_LONG(&const_ATTR_DRIVER_NAME_value, LONG_CONST(PDO_ATTR_DRIVER_NAME));
	zend_string *const_ATTR_DRIVER_NAME_name = zend_string_init_interned("ATTR_DRIVER_NAME", sizeof("ATTR_DRIVER_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_DRIVER_NAME_name, &const_ATTR_DRIVER_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_DRIVER_NAME_name);

	zval const_ATTR_STRINGIFY_FETCHES_value;
	ZVAL_LONG(&const_ATTR_STRINGIFY_FETCHES_value, LONG_CONST(PDO_ATTR_STRINGIFY_FETCHES));
	zend_string *const_ATTR_STRINGIFY_FETCHES_name = zend_string_init_interned("ATTR_STRINGIFY_FETCHES", sizeof("ATTR_STRINGIFY_FETCHES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_STRINGIFY_FETCHES_name, &const_ATTR_STRINGIFY_FETCHES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_STRINGIFY_FETCHES_name);

	zval const_ATTR_MAX_COLUMN_LEN_value;
	ZVAL_LONG(&const_ATTR_MAX_COLUMN_LEN_value, LONG_CONST(PDO_ATTR_MAX_COLUMN_LEN));
	zend_string *const_ATTR_MAX_COLUMN_LEN_name = zend_string_init_interned("ATTR_MAX_COLUMN_LEN", sizeof("ATTR_MAX_COLUMN_LEN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_MAX_COLUMN_LEN_name, &const_ATTR_MAX_COLUMN_LEN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_MAX_COLUMN_LEN_name);

	zval const_ATTR_EMULATE_PREPARES_value;
	ZVAL_LONG(&const_ATTR_EMULATE_PREPARES_value, LONG_CONST(PDO_ATTR_EMULATE_PREPARES));
	zend_string *const_ATTR_EMULATE_PREPARES_name = zend_string_init_interned("ATTR_EMULATE_PREPARES", sizeof("ATTR_EMULATE_PREPARES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_EMULATE_PREPARES_name, &const_ATTR_EMULATE_PREPARES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_EMULATE_PREPARES_name);

	zval const_ATTR_DEFAULT_FETCH_MODE_value;
	ZVAL_LONG(&const_ATTR_DEFAULT_FETCH_MODE_value, LONG_CONST(PDO_ATTR_DEFAULT_FETCH_MODE));
	zend_string *const_ATTR_DEFAULT_FETCH_MODE_name = zend_string_init_interned("ATTR_DEFAULT_FETCH_MODE", sizeof("ATTR_DEFAULT_FETCH_MODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_DEFAULT_FETCH_MODE_name, &const_ATTR_DEFAULT_FETCH_MODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_DEFAULT_FETCH_MODE_name);

	zval const_ATTR_DEFAULT_STR_PARAM_value;
	ZVAL_LONG(&const_ATTR_DEFAULT_STR_PARAM_value, LONG_CONST(PDO_ATTR_DEFAULT_STR_PARAM));
	zend_string *const_ATTR_DEFAULT_STR_PARAM_name = zend_string_init_interned("ATTR_DEFAULT_STR_PARAM", sizeof("ATTR_DEFAULT_STR_PARAM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_DEFAULT_STR_PARAM_name, &const_ATTR_DEFAULT_STR_PARAM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_DEFAULT_STR_PARAM_name);

	zval const_ERRMODE_SILENT_value;
	ZVAL_LONG(&const_ERRMODE_SILENT_value, LONG_CONST(PDO_ERRMODE_SILENT));
	zend_string *const_ERRMODE_SILENT_name = zend_string_init_interned("ERRMODE_SILENT", sizeof("ERRMODE_SILENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERRMODE_SILENT_name, &const_ERRMODE_SILENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERRMODE_SILENT_name);

	zval const_ERRMODE_WARNING_value;
	ZVAL_LONG(&const_ERRMODE_WARNING_value, LONG_CONST(PDO_ERRMODE_WARNING));
	zend_string *const_ERRMODE_WARNING_name = zend_string_init_interned("ERRMODE_WARNING", sizeof("ERRMODE_WARNING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERRMODE_WARNING_name, &const_ERRMODE_WARNING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERRMODE_WARNING_name);

	zval const_ERRMODE_EXCEPTION_value;
	ZVAL_LONG(&const_ERRMODE_EXCEPTION_value, LONG_CONST(PDO_ERRMODE_EXCEPTION));
	zend_string *const_ERRMODE_EXCEPTION_name = zend_string_init_interned("ERRMODE_EXCEPTION", sizeof("ERRMODE_EXCEPTION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERRMODE_EXCEPTION_name, &const_ERRMODE_EXCEPTION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERRMODE_EXCEPTION_name);

	zval const_CASE_NATURAL_value;
	ZVAL_LONG(&const_CASE_NATURAL_value, LONG_CONST(PDO_CASE_NATURAL));
	zend_string *const_CASE_NATURAL_name = zend_string_init_interned("CASE_NATURAL", sizeof("CASE_NATURAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CASE_NATURAL_name, &const_CASE_NATURAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CASE_NATURAL_name);

	zval const_CASE_LOWER_value;
	ZVAL_LONG(&const_CASE_LOWER_value, LONG_CONST(PDO_CASE_LOWER));
	zend_string *const_CASE_LOWER_name = zend_string_init_interned("CASE_LOWER", sizeof("CASE_LOWER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CASE_LOWER_name, &const_CASE_LOWER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CASE_LOWER_name);

	zval const_CASE_UPPER_value;
	ZVAL_LONG(&const_CASE_UPPER_value, LONG_CONST(PDO_CASE_UPPER));
	zend_string *const_CASE_UPPER_name = zend_string_init_interned("CASE_UPPER", sizeof("CASE_UPPER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CASE_UPPER_name, &const_CASE_UPPER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CASE_UPPER_name);

	zval const_NULL_NATURAL_value;
	ZVAL_LONG(&const_NULL_NATURAL_value, LONG_CONST(PDO_NULL_NATURAL));
	zend_string *const_NULL_NATURAL_name = zend_string_init_interned("NULL_NATURAL", sizeof("NULL_NATURAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NULL_NATURAL_name, &const_NULL_NATURAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NULL_NATURAL_name);

	zval const_NULL_EMPTY_STRING_value;
	ZVAL_LONG(&const_NULL_EMPTY_STRING_value, LONG_CONST(PDO_NULL_EMPTY_STRING));
	zend_string *const_NULL_EMPTY_STRING_name = zend_string_init_interned("NULL_EMPTY_STRING", sizeof("NULL_EMPTY_STRING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NULL_EMPTY_STRING_name, &const_NULL_EMPTY_STRING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NULL_EMPTY_STRING_name);

	zval const_NULL_TO_STRING_value;
	ZVAL_LONG(&const_NULL_TO_STRING_value, LONG_CONST(PDO_NULL_TO_STRING));
	zend_string *const_NULL_TO_STRING_name = zend_string_init_interned("NULL_TO_STRING", sizeof("NULL_TO_STRING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NULL_TO_STRING_name, &const_NULL_TO_STRING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NULL_TO_STRING_name);

	zval const_ERR_NONE_value;
	zend_string *const_ERR_NONE_value_str = zend_string_init(PDO_ERR_NONE, strlen(PDO_ERR_NONE), 1);
	ZVAL_STR(&const_ERR_NONE_value, const_ERR_NONE_value_str);
	zend_string *const_ERR_NONE_name = zend_string_init_interned("ERR_NONE", sizeof("ERR_NONE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERR_NONE_name, &const_ERR_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_ERR_NONE_name);

	zval const_FETCH_ORI_NEXT_value;
	ZVAL_LONG(&const_FETCH_ORI_NEXT_value, LONG_CONST(PDO_FETCH_ORI_NEXT));
	zend_string *const_FETCH_ORI_NEXT_name = zend_string_init_interned("FETCH_ORI_NEXT", sizeof("FETCH_ORI_NEXT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_ORI_NEXT_name, &const_FETCH_ORI_NEXT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_ORI_NEXT_name);

	zval const_FETCH_ORI_PRIOR_value;
	ZVAL_LONG(&const_FETCH_ORI_PRIOR_value, LONG_CONST(PDO_FETCH_ORI_PRIOR));
	zend_string *const_FETCH_ORI_PRIOR_name = zend_string_init_interned("FETCH_ORI_PRIOR", sizeof("FETCH_ORI_PRIOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_ORI_PRIOR_name, &const_FETCH_ORI_PRIOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_ORI_PRIOR_name);

	zval const_FETCH_ORI_FIRST_value;
	ZVAL_LONG(&const_FETCH_ORI_FIRST_value, LONG_CONST(PDO_FETCH_ORI_FIRST));
	zend_string *const_FETCH_ORI_FIRST_name = zend_string_init_interned("FETCH_ORI_FIRST", sizeof("FETCH_ORI_FIRST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_ORI_FIRST_name, &const_FETCH_ORI_FIRST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_ORI_FIRST_name);

	zval const_FETCH_ORI_LAST_value;
	ZVAL_LONG(&const_FETCH_ORI_LAST_value, LONG_CONST(PDO_FETCH_ORI_LAST));
	zend_string *const_FETCH_ORI_LAST_name = zend_string_init_interned("FETCH_ORI_LAST", sizeof("FETCH_ORI_LAST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_ORI_LAST_name, &const_FETCH_ORI_LAST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_ORI_LAST_name);

	zval const_FETCH_ORI_ABS_value;
	ZVAL_LONG(&const_FETCH_ORI_ABS_value, LONG_CONST(PDO_FETCH_ORI_ABS));
	zend_string *const_FETCH_ORI_ABS_name = zend_string_init_interned("FETCH_ORI_ABS", sizeof("FETCH_ORI_ABS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_ORI_ABS_name, &const_FETCH_ORI_ABS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_ORI_ABS_name);

	zval const_FETCH_ORI_REL_value;
	ZVAL_LONG(&const_FETCH_ORI_REL_value, LONG_CONST(PDO_FETCH_ORI_REL));
	zend_string *const_FETCH_ORI_REL_name = zend_string_init_interned("FETCH_ORI_REL", sizeof("FETCH_ORI_REL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FETCH_ORI_REL_name, &const_FETCH_ORI_REL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FETCH_ORI_REL_name);

	zval const_CURSOR_FWDONLY_value;
	ZVAL_LONG(&const_CURSOR_FWDONLY_value, LONG_CONST(PDO_CURSOR_FWDONLY));
	zend_string *const_CURSOR_FWDONLY_name = zend_string_init_interned("CURSOR_FWDONLY", sizeof("CURSOR_FWDONLY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CURSOR_FWDONLY_name, &const_CURSOR_FWDONLY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CURSOR_FWDONLY_name);

	zval const_CURSOR_SCROLL_value;
	ZVAL_LONG(&const_CURSOR_SCROLL_value, LONG_CONST(PDO_CURSOR_SCROLL));
	zend_string *const_CURSOR_SCROLL_name = zend_string_init_interned("CURSOR_SCROLL", sizeof("CURSOR_SCROLL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CURSOR_SCROLL_name, &const_CURSOR_SCROLL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CURSOR_SCROLL_name);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "__construct", sizeof("__construct") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	return class_entry;
}
