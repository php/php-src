--TEST--
Base16 decode with spaces and Forgiving mode
--FILE--
<?php
use Encoding\Base16;
use Encoding\DecodingMode;
use function Encoding\base16_decode;

var_dump(base16_decode("48 65\n6C\t6C\r6F 2c 20 57 6f 72 6C 64 21", variant: Base16::Upper, decodingMode: DecodingMode::Forgiving));
?>
--EXPECT--
string(12) "Hello world!"
