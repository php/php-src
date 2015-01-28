--TEST--
Bigints with serialize() and unserialize()
--FILE--
<?php

// an absurdly large value that's going to be a bigint on both 32-bit and 64-bit
$x = 10000000000000000000000000000000000000000000000000000000000000;
var_dump($x);

$serialized_x = serialize($x);
var_dump($serialized_x);

$unserialized_x = unserialize($serialized_x);
var_dump($unserialized_x);

var_dump($x === $unserialized_x);
--EXPECT--
int(10000000000000000000000000000000000000000000000000000000000000)
string(65) "i:10000000000000000000000000000000000000000000000000000000000000;"
int(10000000000000000000000000000000000000000000000000000000000000)
bool(true)
