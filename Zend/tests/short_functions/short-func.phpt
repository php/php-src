--TEST--
Basic short functions return values.
--FILE--
<?php

fn test(int $a) => $a + 1;

fn test2(int $b): int => $b + 1;

print test(5) . PHP_EOL;
print test2(5) . PHP_EOL;

?>
--EXPECT--
6
6
