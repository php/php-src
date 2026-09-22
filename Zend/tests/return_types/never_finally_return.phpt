--TEST--
never return type: never cannot return from finally
--FILE--
<?php

function foo() : never {
    try {
        throw new Exception('bad');
    } finally {
        return;
    }
}

// Note the lack of function call: function validated at compile-time
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d

Fatal error: A never-returning function must not return in %s on line %d
