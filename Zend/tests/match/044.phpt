--TEST--
Match expression can be used as a constant expression unless parts aren't constant
--FILE--
<?php

// Here, this test uses strtolower because the locale dependence and change to the string
// ensures PHP has to use a temporary value instead of a literal

const X = match ($x) {
    default => 123,
};
?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s044.php on line 6
