--TEST--
#[\NoDiscard]: execute_ex overwritten
--EXTENSIONS--
zend_test
--INI--
zend_test.replace_zend_execute_ex=1
opcache.jit=disable
--FILE--
<?php

#[\NoDiscard]
function test(): int {
	return 0;
}

test();

?>
--EXPECTF--
Warning: (G)The return value of function test() is expected to be consumed in %s on line %d
