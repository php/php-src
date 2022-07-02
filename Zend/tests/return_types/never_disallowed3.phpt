--TEST--
never return type: unacceptable cases: implicit return
--FILE--
<?php

function foo(): never {
    if (false) {
        throw new Exception('bad');
    }
}

foo();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: foo(): never-returning function must not implicitly return in %s:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}
  thrown in %s on line %d
