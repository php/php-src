--TEST--
Base16 decode lowercase with Forgiving mode
--FILE--
<?php
use Encoding\Base16;
use Encoding\DecodingMode;
use function Encoding\base16_decode;

var_dump(base16_decode("48656c6c6f20776f726c6421", variant: Base16::Upper, decodingMode: DecodingMode::Forgiving));
?>
--EXPECT--
string(12) "Hello world!"
