--TEST--
round() rejects invalid rounding modes.
--FILE--
<?php
try {
    var_dump(round(1.5, mode: 1234));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: round(): Argument #3 ($mode) must be a valid rounding mode (RoundingMode::*)
