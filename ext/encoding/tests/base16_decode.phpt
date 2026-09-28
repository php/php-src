--TEST--
Base16 decode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base16;

use function Encoding\base16_decode;

$encodedUpper = "48656C6C6F20776F726C6421";
$encodedLower = "48656c6c6f20776f726c6421";

var_dump(base16_decode($encodedUpper));

var_dump(base16_decode($encodedLower, variant: Base16::Lower));

?>
--EXPECT--
string(12) "Hello world!"
string(12) "Hello world!"
