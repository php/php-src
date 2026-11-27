--TEST--
#[\NoDiscard]: execute_ex overwritten (Class).
--EXTENSIONS--
zend_test
--INI--
zend_test.replace_zend_execute_ex=1
opcache.jit=disable
--FILE--
<?php

#[\NoDiscard]
class Test { }
function Test() { return new (__FUNCTION__)(); }

Test();

?>
--EXPECTF--
Warning: The returned object of class Test should either be used or intentionally ignored by casting it as (void) in %s on line %d
