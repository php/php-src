--TEST--
void return type: disallowed (1)
--FILE--
<?php

function foo(): void {
    return NULL; // explicit NULL return is not permitted
}

foo();

--EXPECTF--
Catchable fatal error: foo() must not return a value, null returned in %svoid2.php on line 4
