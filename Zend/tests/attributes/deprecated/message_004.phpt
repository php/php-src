--TEST--
#[\Deprecated]: Message with value unknown at compile time.
--EXTENSIONS--
zend_test
--FILE--
<?php

define('MESSAGE', 'value-' . zend_return_non_interned_string('a'));

#[\Deprecated(MESSAGE)]
function test() {
}

test();

?>
--EXPECTF--
Deprecated: Function test() is deprecated, value-a in %s on line %d
