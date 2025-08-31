/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 07475caecc81ab3b38a04905f874615af1126289 */

static zend_class_entry *register_class_LogicException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "LogicException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	return class_entry;
}

static zend_class_entry *register_class_BadFunctionCallException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "BadFunctionCallException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_LogicException, 0);

	return class_entry;
}

static zend_class_entry *register_class_BadMethodCallException(zend_class_entry *class_entry_BadFunctionCallException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "BadMethodCallException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_BadFunctionCallException, 0);

	return class_entry;
}

static zend_class_entry *register_class_DomainException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DomainException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_LogicException, 0);

	return class_entry;
}

static zend_class_entry *register_class_InvalidArgumentException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "InvalidArgumentException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_LogicException, 0);

	return class_entry;
}

static zend_class_entry *register_class_LengthException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "LengthException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_LogicException, 0);

	return class_entry;
}

static zend_class_entry *register_class_OutOfRangeException(zend_class_entry *class_entry_LogicException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OutOfRangeException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_LogicException, 0);

	return class_entry;
}

static zend_class_entry *register_class_RuntimeException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RuntimeException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	return class_entry;
}

static zend_class_entry *register_class_OutOfBoundsException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OutOfBoundsException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_RuntimeException, 0);

	return class_entry;
}

static zend_class_entry *register_class_OverflowException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OverflowException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_RuntimeException, 0);

	return class_entry;
}

static zend_class_entry *register_class_RangeException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RangeException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_RuntimeException, 0);

	return class_entry;
}

static zend_class_entry *register_class_UnderflowException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "UnderflowException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_RuntimeException, 0);

	return class_entry;
}

static zend_class_entry *register_class_UnexpectedValueException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "UnexpectedValueException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_RuntimeException, 0);

	return class_entry;
}
