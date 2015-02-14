--TEST--
void return type: disallowed (2)
--FILE--
<?php

function foo(): void {
    return 1;
}

foo();

--EXPECTF--
Catchable fatal error: foo() must not return a value, integer returned in %svoid3.php on line 4
