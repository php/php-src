--TEST--
Base16 decode lowercase with Lower variant
--FILE--
<?php
use Encoding\Base16;
use function Encoding\base16_decode;

var_dump(base16_decode("48656c6c6f20776f726c6421", variant: Base16::Lower));
?>
--EXPECT--
string(12) "Hello world!"
