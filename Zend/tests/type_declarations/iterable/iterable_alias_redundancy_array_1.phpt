--TEST--
iterable type with array should be redundant
--FILE--
<?php

function bar(): array|iterable|null {
    return null;
}

?>
--EXPECTF--
Fatal error: Duplicate type array is redundant in %s on line %d
