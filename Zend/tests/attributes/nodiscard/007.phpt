--TEST--
#[\NoDiscard]: execute_internal overwritten
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.execute_internal=1
--FILE--
<?php

zend_test_nodiscard();

?>
--EXPECTF--
<!-- internal enter NoDiscard::__construct() -->

Warning: The return value of function zend_test_nodiscard() should either be used or intentionally ignored by casting it as (void), custom message in %s on line %d
<!-- internal enter zend_test_nodiscard() -->
