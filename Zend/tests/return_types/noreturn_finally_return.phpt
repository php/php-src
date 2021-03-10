--TEST--
noreturn return type: noreturn cannot return from finally
--FILE--
<?php

function foo() : noreturn {
    try {
        throw new Exception('bad');
    } finally {
        return;
    }
}

// Note the lack of function call: function validated at compile-time
?>
--EXPECTF--
Fatal error: A noreturn function must not return in %s on line %d
