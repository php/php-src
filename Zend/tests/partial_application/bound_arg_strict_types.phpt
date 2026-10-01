--TEST--
Partial application checks bound arguments with the creating scope's strict_types
--FILE--
<?php
declare(strict_types=1);

function f(int $a, $b) { return $a; }

try {
    f("3", ?);
} catch (\TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

function make() {
    try {
        f("3", ?);
    } catch (\TypeError $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}
make();
?>
--EXPECT--
TypeError: f(): Argument #1 ($a) must be of type int, string given
TypeError: f(): Argument #1 ($a) must be of type int, string given
