--TEST--
Base16 decode forgiving mode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base16;
use Encoding\DecodingMode;

use function Encoding\base16_decode;

$encodedLower = "48656c6c6f20776f726c6421";
$encodedUpperWithSpaces = "48 65\n6C\t6C\r6F 20 77 6F 72 6C 64 21";
$encodedWithSpaces = "48 65\n6C\t6C\r6F 20 77 6f 72 6C 64 21";

var_dump(base16_decode($encodedLower, variant: Base16::Upper, decodingMode: DecodingMode::Forgiving));

var_dump(base16_decode($encodedUpperWithSpaces));

var_dump(base16_decode($encodedWithSpaces, variant: Base16::Upper, decodingMode: DecodingMode::Forgiving));

?>
--EXPECT--
string(12) "Hello world!"
string(12) "Hello world!"
string(12) "Hello world!"
