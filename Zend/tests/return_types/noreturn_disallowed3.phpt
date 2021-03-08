--TEST--
noreturn return type: unacceptable cases: empty return
--FILE--
<?php

function foo(): noreturn {
    if (false) {
        throw new Exception('bad');
    }
}

foo();

// Note the lack of function call: function validated at compile-time
?>
--EXPECTF--
Fatal error: Uncaught TypeError: foo(): Nothing was expected to be returned in %s:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}
  thrown in %s on line %d
