--TEST--
Base16 decode uppercase
--FILE--
<?php
use function Encoding\base16_decode;

var_dump(base16_decode("48656C6C6F20776F726C6421"));
?>
--EXPECT--
string(12) "Hello world!"
