--TEST--
iterable type with second iterable should be redundant
--FILE--
<?php

function bar(): iterable|iterable|null {
    return null;
}

?>
--EXPECTF--
Fatal error: Duplicate type array is redundant in %s on line %d
