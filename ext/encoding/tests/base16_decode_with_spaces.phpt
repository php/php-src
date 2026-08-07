--TEST--
Base16 decode uppercase with spaces
--FILE--
<?php
use function Encoding\base16_decode;

var_dump(base16_decode("48 65\n6C\t6C\r6F 2C 20 57 6F 72 6C 64 21"));
?>
--EXPECT--
string(12) "Hello world!"
