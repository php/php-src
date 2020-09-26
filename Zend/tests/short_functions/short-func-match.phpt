--TEST--
Short functions with match statements.
--FILE--
<?php

function pick_one(int $a) => match($a) {
    1 => 'One',
    2 => 'Two',
    3 => 'Three',
    default => 'More',
};

print pick_one(1) . PHP_EOL;
print pick_one(2) . PHP_EOL;
print pick_one(3) . PHP_EOL;
print pick_one(4) . PHP_EOL;

?>
--EXPECT--
One
Two
Three
More
