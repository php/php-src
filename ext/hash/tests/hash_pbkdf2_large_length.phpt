--TEST--
Hash: hash_pbkdf2() function : large output length
--FILE--
<?php

$length = 33554433;
$hash = hash_pbkdf2('md5', 'password', 'salt', 1, $length);

/* The last hexit comes from the first byte of the final PBKDF2 block. */
$block = intdiv(intdiv($length + 1, 2) - 1, 16) + 1;
$expected = bin2hex(hash_hmac('md5', 'salt' . pack('N', $block), 'password', true));

var_dump(strlen($hash));
var_dump($hash[$length - 1] === $expected[0]);

?>
--EXPECT--
int(33554433)
bool(true)
