--TEST--
Base64 encoding with 0xFF bytes
--FILE--
<?php
use function Encoding\base64_encode;

$data = chr(0xFF) . chr(0xFF);
echo base64_encode($data) . "\n";
echo base64_encode($data, variant: \Encoding\Base64::UrlSafe) . "\n";
echo base64_encode($data, paddingMode: \Encoding\PaddingMode::StripPadding) . "\n";
?>
--EXPECT--
//8=
__8
//8
