--TEST--
Base32 decode default ASCII
--FILE--
<?php
use function Encoding\base32_decode;

var_dump(base32_decode("JBSWY3DPEBLW64TMMQ======"));
?>
--EXPECT--
string(12) "Hello world!"
