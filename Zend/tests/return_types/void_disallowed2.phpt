--TEST--
void return type: unacceptable cases: explicit return of some other value
--FILE--
<?php

function foo(): void {
    return -1; // not permitted in a void function
}

// Note the lack of function call: function validated at compile-time
--EXPECTF--
Fatal error: Function foo() with return type void must not return a value in %s on line %d
