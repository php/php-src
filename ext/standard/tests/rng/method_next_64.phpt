--TEST--
Test method: next64() call.
--FILE--
<?php
foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $rng = $is_seed ? new $class(\random_int(PHP_INT_MIN, PHP_INT_MAX)) : new $class();
    for ($i = 1; $i <= 10000; $i++) {
        $next = $rng->next64();
    }
}

die('OK.');
?>
--EXPECT--
OK.
