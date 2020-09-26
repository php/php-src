--TEST--
Basic short functions return values.
--FILE--
<?php

function test(int $a) => $a + 1;

function test2(int $b) => return $b . 'error';

print test(5);

?>
--EXPECT--
6
