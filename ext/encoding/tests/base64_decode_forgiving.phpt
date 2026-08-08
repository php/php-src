--TEST--
Base64 decode forgiving mode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\DecodingMode;

use function Encoding\base64_decode;

var_dump(base64_decode("VGhpcyBpcyBhbiBlbmNvZGVkIHN0cmluZw", decodingMode: DecodingMode::Forgiving));

?>
--EXPECT--
string(25) "This is an encoded string"
