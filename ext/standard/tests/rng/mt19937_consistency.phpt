--TEST--
Test class: MT19937: consistent for mt_srand() / mt_rand()
--FILE--
<?php

for ($i = 0; $i < 100; $i++) {
    $seed = \random_int(PHP_INT_MIN, PHP_INT_MAX);
    $rng = new \RNG\MT19937($seed);
    \mt_srand($seed);
    for ($j = 0; $j < 1000; $j++) {
        $rng_next = \rng_next($rng);
        $func_next = \mt_rand();

        if ($rng_next !== $func_next) {
            die("NG, Inconsistent result MT19937: ${rng_next} mt_rand: ${func_next} i: ${i} j: ${j}");
        }
    }
    // shuffle
    $rng_array = \range(0, 99);
    $func_array = \range(0, 99);
    \shuffle($rng_array, $rng);
    \shuffle($func_array);
    for ($k = 0; $k < 100; $k++) {
        if ($rng_array[$k] !== $func_array[$k]) {
            die("NG, Incosistent result MT19937: {$rng_array[$k]} shuffle: {$func_array[$k]} i: ${i} k: ${k}");
        }
    }
    // str_shuffle
    $rng_string = \random_bytes(100);
    $func_string = $rng_string;
    $rng_string = \str_shuffle($rng_string);
    $func_string = \str_shuffle($func_string, $rng);
    for ($k = 0; $k < 100; $k++) {
        if ($rng_string[$k] !== $func_string[$k]) {
            die("NG, Incosistent result MT19937: {$rng_string[$k]} str_shuffle: {$func_string[$k]} i: ${i} k: ${k}");
        }
    }
    // array_rand (1 key)
    $rng_data = ['a' => 0, 'b' => 1, 'c' => 2, 'd' => 3, 'e' => 4, 'f' => 5, 'g' => 6, 'h' => 7, 'i' => 8, 'j' => 9, 'k' => 10, 'l' => 11, 'm' => 12, 'n' => 13, 'o' => 14, 'p' => 15, 'q' => 16, 'r' => 17, 's' => 18, 't' => 19, 'u' => 20, 'v' => 21, 'w' => 22, 'x' => 23, 'y' => 24, 'z' => 25];
    $func_data = $rng_data;
    for ($k = 0; $k < 26; $k++) {
        $rng_result = \array_rand($rng_data, 1, $rng);
        $func_result = \array_rand($func_data, 1);
        if ($rng_result !== $func_result) {
            die("NG, Incosistent result MT19937: {$rng_result} array_rand (1 key): {$func_result} i: ${i} k: ${k}");
        }
    }
    // array_rand (2 keys)
    $rng_data = ['a' => 0, 'b' => 1, 'c' => 2, 'd' => 3, 'e' => 4, 'f' => 5, 'g' => 6, 'h' => 7, 'i' => 8, 'j' => 9, 'k' => 10, 'l' => 11, 'm' => 12, 'n' => 13, 'o' => 14, 'p' => 15, 'q' => 16, 'r' => 17, 's' => 18, 't' => 19, 'u' => 20, 'v' => 21, 'w' => 22, 'x' => 23, 'y' => 24, 'z' => 25];
    $func_data = $rng_data;
    for ($k = 0; $k < 26; $k++) {
        [$rng_result1, $rng_result2] = \array_rand($rng_data, 2, $rng);
        [$func_result1, $func_result2] = \array_rand($func_data, 2);
        if ($rng_result1 !== $func_result1 || $rng_result2 !== $func_result2) {
            die("NG, Incosistent result MT19937: {$rng_result1} / ${rng_result2} array_rand (2 key): {$func_result1} / ${func_result2} i: ${i} k: ${k}");
        }
    }
}
die('OK, Result is consistent.');
?>
--EXPECT--
OK, Result is consistent.
