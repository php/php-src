--TEST--
GH-18037 (SEGV Zend/zend_execute.c)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1201
--FILE--
<?php
function test_helper()
{
    $list = [];
    \in_array($list[0], $list, true) !== $list->matches();
}

test_helper();
?>
--EXPECTF--
Warning: Undefined array key 0 in %s on line %d

Fatal error: Uncaught Error: Call to a member function matches() on array in %s:%d
Stack trace:
#0 %s(%d): test_helper()
#1 {main}
  thrown in %s on line %d
