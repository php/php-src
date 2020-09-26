--TEST--
Basic short functions return values.
--FILE--
<?php

function test(int $a) => $a + 1;

function test2(int $b)
    => $b + 1;

print test(5) . PHP_EOL;
print test2(5) . PHP_EOL;

?>
--EXPECT--
6
6
