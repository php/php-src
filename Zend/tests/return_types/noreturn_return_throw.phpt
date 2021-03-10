--TEST--
noreturn return type: noreturn cannot return from throw expression
--FILE--
<?php

function foo() : noreturn {
    return throw new Exception('bad');
}

// Note the lack of function call: function validated at compile-time
?>
--EXPECTF--
Fatal error: A noreturn function must not return in %s on line %d
