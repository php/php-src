--TEST--
Base64 decode missing padding with Forgiving mode
--FILE--
<?php
use Encoding\DecodingMode;
use function Encoding\base64_decode;

var_dump(base64_decode("VGhpcyBpcyBhbiBlbmNvZGVkIHN0cmluZw", decodingMode: DecodingMode::Forgiving));
?>
--EXPECT--
string(25) "This is an encoded string"
