--TEST--
Base32 decode missing padding with Forgiving mode
--FILE--
<?php
use Encoding\DecodingMode;
use function Encoding\base32_decode;

var_dump(base32_decode("JBSWY3DPEBLW64TMMQ", decodingMode: DecodingMode::Forgiving));
?>
--EXPECT--
string(12) "Hello world!"
