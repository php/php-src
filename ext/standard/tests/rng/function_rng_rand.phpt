--TEST--
Test function: rng_rand() function.
--FILE--
<?php

const SEED = 1234;

foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $rng = $is_seed ? new $class(SEED) : new $class();
    foreach (range(0, 999) as $i) {
        $next = rng_rand($rng, 1, 10000);

        if ($next < 0 || $next > 10000) {
            die("NG, invalid number: ${next}.");
        }
    }
}
die('OK, rng_rand() works correctly.');
?>
--EXPECT--
OK, rng_rand() works correctly.
