--TEST--
Test function: rng_int() function in 64bit range.
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    echo 'skip this test only available on 64bit enviroment.';
}
?>
--FILE--
<?php

const SEED = 1234;

foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $rng = $is_seed ? new $class(SEED) : new $class();
    foreach (\range(0, 999) as $i) {
        $next = \rng_int($rng, 4294967296, PHP_INT_MAX);

        if ($next < 4294967296) {
            die("NG, invalid number: ${next}.");
        }
    }
}
die('OK, rng_int() works correctly on 64bit.');
?>
--EXPECT--
OK, rng_int() works correctly on 64bit.
