--TEST--
Bug #70241 (Skipped assertions affect Generator returns)
--INI--
zend.assertions=-1
--FILE--
<?php

function foo () {
	assert(yield 1);
	return null;
}

var_dump(foo() instanceof Generator);

?>
--EXPECT--
bool(true)
