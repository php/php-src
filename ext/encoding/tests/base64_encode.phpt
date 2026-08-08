--TEST--
Base64 encode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base64;
use Encoding\PaddingMode;

use function Encoding\base64_encode;

$data = 'This is an encoded string';

echo base64_encode($data);
echo "\n";

echo base64_encode($data, paddingMode: PaddingMode::StripPadding);
echo "\n";

$data = chr(0xFF) . chr(0xFF);
echo base64_encode($data);
echo "\n";

echo base64_encode($data, variant: Base64::UrlSafe);
echo "\n";

echo base64_encode($data, paddingMode: PaddingMode::StripPadding);
echo "\n";

?>
--EXPECT--
VGhpcyBpcyBhbiBlbmNvZGVkIHN0cmluZw==
VGhpcyBpcyBhbiBlbmNvZGVkIHN0cmluZw
//8=
__8
//8
