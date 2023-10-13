--TEST--
Nested match expressions
--FILE--
<?php

// Nested in return expression:
$b = 'b';
echo match($b) {
    'a' => 1,
    'b' => match (1) {
        strpos('ab', $b) => 100,
        default => 15
    },
    default => 4
};

echo "\n";

// Nested in condition expression:
$c = 'c';
echo match($c) {
    'foo' => 'bar',
    match ($c) { default => 'c' } => 300
};

echo "\n";

// Nested in one of many condition expressions:
$d = 'd';
echo match($d) {
    'foo' => 'bar',
    match ($d) { default => 'd' }, match ($d) { default => 'e' } => 500
};


?>
--EXPECT--
100
300
500
