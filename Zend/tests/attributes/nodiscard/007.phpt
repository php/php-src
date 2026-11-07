--TEST--
#[\NoDiscard]: execute_internal overwritten
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.execute_internal=1
--FILE--
<?php

define('MSG', 'from a constant');

#[\NoDiscard(message: MSG)]
function f() { return 1; }

zend_test_nodiscard();
f();

?>
--EXPECTF--
<!-- internal enter define() -->
<!-- internal leave define() -->

Warning: The return value of function zend_test_nodiscard() should either be used or intentionally ignored by casting it as (void), custom message in %s on line %d
<!-- internal enter zend_test_nodiscard() -->
<!-- internal leave zend_test_nodiscard() -->
<!-- internal enter NoDiscard::__construct() -->
<!-- internal leave NoDiscard::__construct() -->

Warning: The return value of function f() should either be used or intentionally ignored by casting it as (void), from a constant in %s on line %d
