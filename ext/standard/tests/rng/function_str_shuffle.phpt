--TEST--
Test function: str_shuffle() function with RNGs.
--FILE--
<?php

const SEED = 1234;

foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_rng_classes.inc') as $class => $is_seed) {
    $string = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.';
    $rng = $is_seed ? new $class(SEED) : new $class();
    $shuffled_string = str_shuffle($string, $rng);
    for ($i = 0; $i < strlen($string); $i++) {
        if ($string[$i] !== $shuffled_string[$i]) {
            continue 2;
        }
    }
    die("NG, String not shuffled. RNG class: ${class} string: ${shuffled_string}");
}
die('OK, All string shuffled.');
?>
--EXPECT--
OK, All string shuffled.
