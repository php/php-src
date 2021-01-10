--TEST--
Test class: inheritance classes.
--FILE--
<?php
const SEED = 1234;

foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $rng = $is_seed ? new $class(SEED) : new $class();
    if (! $rng instanceof \RNG\RNGInterface) {
        die("NG, ${class} has not implemented \\RNG\\RNGInterface.");
    }
    if (! $rng instanceof \RNG\RNG64Interface) {
        die("NG, ${class} has not implemented \\RNG\\RNG64Interface.");
    }
}

die('OK, inheritance is corrected.');
?>
--EXPECT--
OK, inheritance is corrected.
