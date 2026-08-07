--TEST--
Base64 encoding with StripPadding
--FILE--
<?php
use Encoding\PaddingMode;
use function Encoding\base64_encode;

echo base64_encode('This is an encoded string', paddingMode: PaddingMode::StripPadding);
echo "\n";
?>
--EXPECT--
VGhpcyBpcyBhbiBlbmNvZGVkIHN0cmluZw
