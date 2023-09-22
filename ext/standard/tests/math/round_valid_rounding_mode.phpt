--TEST--
round() rejects invalid rounding modes.
--FILE--
<?php
try {
    var_dump(round(1.5, mode: 1234));
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
round(): Argument #3 ($mode) must be a valid rounding mode (PHP_ROUND_*)
