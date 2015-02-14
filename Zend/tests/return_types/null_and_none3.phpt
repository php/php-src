--TEST--
Return types none/null distinction: return null;
--FILE--
<?php

function returns_null(): array {
    return null;
}

returns_null();
--EXPECTF--
Catchable fatal error: Return value of returns_null() must be of the type array, null returned in %snull_and_none3.php on line 4
