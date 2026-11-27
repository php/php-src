--TEST--
#[\NoDiscard]: Basic test (Class).
--FILE--
<?php

#[\NoDiscard]
class Test { }
function Test() { return new (__FUNCTION__)(); }

#[\NoDiscard("this is important")]
class Test2 { }
function Test2() { return new (__FUNCTION__)(); }

Test();
Test2();

array_first([new Test(), new Test2()]);
array_last([new Test(), new Test2()]);

?>
--EXPECTF--
Warning: The returned object of class Test should either be used or intentionally ignored by casting it as (void) in %s on line %d

Warning: The returned object of class Test2 should either be used or intentionally ignored by casting it as (void), this is important in %s on line %d

Warning: The returned object of class Test should either be used or intentionally ignored by casting it as (void) in %s on line %d

Warning: The returned object of class Test2 should either be used or intentionally ignored by casting it as (void), this is important in %s on line %d
