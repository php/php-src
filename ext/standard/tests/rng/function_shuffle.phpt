--TEST--
Test function: shuffle() function with RNGs.
--FILE--
<?php

const SEED = 1234;

foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $array = range(0, 999);
    $rng = $is_seed ? new $class(SEED) : new $class();
    shuffle($array, $rng);
    foreach (range(0, 999) as $i) {
        foreach ($array as $key => $value) {
            if ($key !== $i) {
                break 2;
            }
        }
        die("NG, Array not shuffled. RNG class: ${class}");
    }
}
die('OK, All array shuffled.');
?>
--EXPECT--
OK, All array shuffled.
