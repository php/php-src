--TEST--
Base32 decode Crockford data with Crockford variant
--FILE--
<?php
use Encoding\Base32;
use function Encoding\base32_decode;

$encodedCrockFord = "91JPRV3F41BPYWKCCG";
var_dump(base32_decode($encodedCrockFord, variant: Base32::Crockford));
?>
--EXPECT--
string(12) "Hello world!"
