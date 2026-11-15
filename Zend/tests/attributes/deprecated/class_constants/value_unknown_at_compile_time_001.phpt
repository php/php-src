--TEST--
#[\Deprecated]: Class constant with value unknown at compile time.
--EXTENSIONS--
zend_test
--FILE--
<?php

define('SUFFIX', zend_return_non_interned_string('a'));

class Clazz {
	#[\Deprecated]
	public const CONSTANT = self::class . '-' . SUFFIX;
}

$value = Clazz::CONSTANT;
var_dump($value);
var_dump($value === 'Clazz-' . SUFFIX);

?>
--EXPECTF--
Deprecated: Constant Clazz::CONSTANT is deprecated in %s on line %d
string(7) "Clazz-a"
bool(true)
