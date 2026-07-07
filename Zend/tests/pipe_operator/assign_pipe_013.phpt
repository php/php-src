--TEST--
Pipe assign operator exception interruption
--FILE--
<?php

function will_throw(int $x): int {
    throw new RuntimeException("interrupted at $x");
}

$x = 42;
try {
    $x |>= (fn($v) => $v * 2) |> will_throw(...);
} catch (RuntimeException $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

var_dump($x);

?>
--EXPECT--
RuntimeException: interrupted at 84
int(42)
