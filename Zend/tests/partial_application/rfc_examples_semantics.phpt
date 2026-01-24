--TEST--
PFA RFC examples: "Placeholder Semantics" section
--FILE--
<?php

require 'rfc_examples.inc';

class Point {}

function foo(int $a = 5, int $b = 1, string ...$c) { }

function stuff(int $i, string $s, float $f, Point $p, int $m = 0) {}

$tests = [
    'Test 1' => [
        foo(?, ?, ?, ?),
        static fn(int $a, int $b, string $c0, string $c1) => foo($a, $b, $c0, $c1),
    ],
    'Test 2' => [
        stuff(1, ?, p: ?, f: 3.14, ...),
        static fn(string $s, Point $p, int $m = 0) => stuff(1, $s, 3.14, $p, $m),
    ],
];

check_equivalence($tests);

?>
--EXPECT--
# Test 1: Ok
# Test 2: Ok
