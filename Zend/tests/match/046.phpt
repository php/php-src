--TEST--
Nested match expressions
--FILE--
<?php

$c = 'c';
echo match($c) {
    'a' => 1,
    'b' => 2,
    'c' => match (1) {
        strpos('zc', $c) => 100,
        default => 15
    },
    default => 4
};

echo "\n";

$b = 'c';
echo match($b) {
    'b' => 200,
    match ($b) { default => 'c' } => 300
};

?>
--EXPECT--
100
300
