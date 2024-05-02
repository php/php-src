/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 07475caecc81ab3b38a04905f874615af1126289 */

static const zend_function_entry class_LogicException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_BadFunctionCallException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_BadMethodCallException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_DomainException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_InvalidArgumentException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_LengthException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_OutOfRangeException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_RuntimeException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_OutOfBoundsException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_OverflowException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_RangeException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_UnderflowException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_UnexpectedValueException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_LogicException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "LogicException", class_LogicException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);

	return class_entry;
}

static zend_class_entry *register_class_BadFunctionCallException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "BadFunctionCallException", class_BadFunctionCallException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_LogicException);

	return class_entry;
}

static zend_class_entry *register_class_BadMethodCallException(zend_class_entry *class_entry_BadFunctionCallException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "BadMethodCallException", class_BadMethodCallException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_BadFunctionCallException);

	return class_entry;
}

static zend_class_entry *register_class_DomainException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DomainException", class_DomainException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_LogicException);

	return class_entry;
}

static zend_class_entry *register_class_InvalidArgumentException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "InvalidArgumentException", class_InvalidArgumentException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_LogicException);

	return class_entry;
}

static zend_class_entry *register_class_LengthException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "LengthException", class_LengthException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_LogicException);

	return class_entry;
}

static zend_class_entry *register_class_OutOfRangeException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OutOfRangeException", class_OutOfRangeException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_LogicException);

	return class_entry;
}

static zend_class_entry *register_class_RuntimeException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RuntimeException", class_RuntimeException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);

	return class_entry;
}

static zend_class_entry *register_class_OutOfBoundsException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OutOfBoundsException", class_OutOfBoundsException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}

static zend_class_entry *register_class_OverflowException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OverflowException", class_OverflowException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}

static zend_class_entry *register_class_RangeException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RangeException", class_RangeException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}

static zend_class_entry *register_class_UnderflowException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "UnderflowException", class_UnderflowException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}

static zend_class_entry *register_class_UnexpectedValueException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "UnexpectedValueException", class_UnexpectedValueException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}
