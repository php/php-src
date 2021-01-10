--TEST--
Test function: array_rand() function with RNGs.
--FILE--
<?php

const SEED = 1234;

// Note: OSRNG fail in rare cases (it's truly random result)
$array = ['a' => 0, 'b' => 1, 'c' => 2, 'd' => 3, 'e' => 4, 'f' => 5, 'g' => 6, 'h' => 7, 'i' => 8, 'j' => 9, 'k' => 10, 'l' => 11, 'm' => 12, 'n' => 13, 'o' => 14, 'p' => 15, 'q' => 16, 'r' => 17, 's' => 18, 't' => 19, 'u' => 20, 'v' => 21, 'w' => 22, 'x' => 23, 'y' => 24, 'z' => 25];
// one key
foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $rng = $is_seed ? new $class(SEED) : new $class();
    foreach ($array as $key => $value) {
        if (array_rand($array, 1, $rng) !== $key) {
            continue 2;
        }
    }
    die("NG, Failed get randomize key. RNG class: ${class}");
}
// more keys.
foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $rng = $is_seed ? new $class(SEED) : new $class();
    [$key1, $key2] = array_rand($array, 2, $rng);
    if (
        ! array_key_exists($key1, $array) ||
        ! array_key_exists($key2, $array) ||
        ($key1 === 'a' && $key2 === 'b')
    ) {
        die("NG, Failed get multiple random keys. RNG class: ${class}");
    }
}
die('OK, All works correctly.');
?>
--EXPECT--
OK, All works correctly.
