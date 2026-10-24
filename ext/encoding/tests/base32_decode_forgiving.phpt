--TEST--
Base32 decode forgiving mode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\DecodingMode;

use function Encoding\base32_decode;

var_dump(base32_decode("JBSWY3DPEB3W64TMMQQQ", decodingMode: DecodingMode::Forgiving));

?>
--EXPECT--
string(12) "Hello world!"
