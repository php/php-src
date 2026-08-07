--TEST--
Base64 encoding default Standard variant
--FILE--
<?php
use function Encoding\base64_encode;

echo base64_encode('This is an encoded string');
echo "\n";
?>
--EXPECT--
VGhpcyBpcyBhbiBlbmNvZGVkIHN0cmluZw==
