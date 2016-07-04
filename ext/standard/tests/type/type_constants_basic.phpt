--TEST--
TYPE_* constants are defined with their correct value
--DESCRIPTION--
This test ensures that users of PHP can rely on the definition of these
constants as well as their actual values and are meant to prevent any kind of
breaking changes in relation to these constants.
--FILE--
<?php

$data = [
	'TYPE_ARRAY'    => 'array',
	'TYPE_BOOL'     => 'bool',
	'TYPE_CALLABLE' => 'callable',
	'TYPE_FLOAT'    => 'float',
	'TYPE_INT'      => 'int',
	'TYPE_ITERABLE' => 'iterable',
	'TYPE_NULL'     => 'null',
	'TYPE_OBJECT'   => 'object',
	'TYPE_RESOURCE' => 'resource',
	'TYPE_STRING'   => 'string',
];

foreach ($data as $name => $value) {
	if (defined($name) && constant($name) === $value) {
		printf("%-13s := %s\n", $name, $value);
	}
}

?>
--EXPECT--
TYPE_ARRAY    := array
TYPE_BOOL     := bool
TYPE_CALLABLE := callable
TYPE_FLOAT    := float
TYPE_INT      := int
TYPE_ITERABLE := iterable
TYPE_NULL     := null
TYPE_OBJECT   := object
TYPE_RESOURCE := resource
TYPE_STRING   := string
