--TEST--
Test function: rng_bytes() function.
--FILE--
<?php

const SEED = 1234;

foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $rng = $is_seed ? new $class(SEED) : new $class();
    foreach (\range(0, 999) as $i) {
        $next = \rng_bytes($rng, 100);

        if (\strlen($next) !== 100) {
            die("NG, invalid bytes: ${next}.");
        }
    }
}
die('OK, rng_bytes() works correctly.');
?>
--EXPECT--
OK, rng_bytes() works correctly.
