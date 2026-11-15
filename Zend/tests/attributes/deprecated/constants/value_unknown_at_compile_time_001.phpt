--TEST--
#[\Deprecated]: Constant with value unknown at compile time.
--EXTENSIONS--
zend_test
--FILE--
<?php

define('SUFFIX', zend_return_non_interned_string('a'));

#[\Deprecated]
const CONSTANT = 'Prefix-' . SUFFIX;

$value = CONSTANT;
var_dump($value);
var_dump($value === 'Prefix-' . SUFFIX);

?>
--EXPECTF--
Deprecated: Constant CONSTANT is deprecated in %s on line %d
string(8) "Prefix-a"
bool(true)
