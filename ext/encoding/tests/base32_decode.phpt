--TEST--
Base32 decode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base32;

use function Encoding\base32_decode;

$encodedAscii = "JBSWY3DPEB3W64TMMQQQ====";
$encodedCrockFord = "91JPRV3F41VPYWKCCGGG";

var_dump(base32_decode($encodedAscii));

var_dump(base32_decode($encodedCrockFord, variant: Base32::Crockford));

?>
--EXPECT--
string(12) "Hello world!"
string(12) "Hello world!"
