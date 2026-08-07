--TEST--
Base32 encoding with StripPadding
--FILE--
<?php
use Encoding\PaddingMode;
use function Encoding\base32_encode;

echo base32_encode('Hello world!', paddingMode: PaddingMode::StripPadding);
echo "\n";
?>
--EXPECT--
JBSWY3DPEBLW64TMMQ
