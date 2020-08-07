--TEST--
Match expression can be used as a constant expression unless parts aren't constant
--FILE--
<?php

// This is an uncatchable fatal error caused by compiling this file.
const X = match ($x) {
    default => 123,
};
?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s044.php on line 4
