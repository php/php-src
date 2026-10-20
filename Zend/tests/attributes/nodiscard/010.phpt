--TEST--
#[\NoDiscard]: Native function.
--EXTENSIONS--
zend_test
--FILE--
<?php

zend_test_nodiscard();

?>
--EXPECTF--
Warning: The return value of function zend_test_nodiscard() should either be used or intentionally ignored by casting it as (void), custom message in %s on line %d
