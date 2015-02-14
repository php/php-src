--TEST--
Return types none/null distinction: return;
--FILE--
<?php

function returns_none(): array {
    return;
}

returns_none();
--EXPECTF--
Catchable fatal error: Return value of returns_none() must be of the type array, no value returned in %snull_and_none2.php on line 4
