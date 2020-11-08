--TEST--
RFC example: cannot return null with a return type declaration
--FILE--
<?php
function foo(): DateTime {
    return null;
}

foo();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: foo(): Return value must be of type DateTime, null returned in %s:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}
  thrown in %s on line %d
