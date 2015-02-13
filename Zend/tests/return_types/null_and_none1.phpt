--TEST--
Return types none/null distinction: Empty function
--FILE--
<?php

function lacks_return(): array {
}

lacks_return();

--EXPECTF--
Catchable fatal error: Return value of lacks_return() must be of the type array, no value returned in %snull_and_none1.php on line 3
