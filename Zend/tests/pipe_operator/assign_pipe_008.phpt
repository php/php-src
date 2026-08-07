--TEST--
Pipe assign operator error: by-reference rejection
--FILE--
<?php

function modify_ref(int &$a): int {
    $a += 1;
    return $a;
}

try {
    $a = 5;
    $a |>= modify_ref(...);
} catch (\Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

var_dump($a);

?>
--EXPECTF--
Error: modify_ref(): Argument #1 ($a) could not be passed by reference
int(5)
