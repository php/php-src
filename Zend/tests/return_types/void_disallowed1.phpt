--TEST--
void return type: unacceptable cases: explicit NULL return
--FILE--
<?php

function foo(): void {
    return NULL; // not permitted in a void function
}

// Note the lack of function call: function validated at compile-time
--EXPECTF--
Fatal error: A void function must not return a value in %s on line %d
