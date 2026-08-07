--TEST--
Base32 encoding ASCII variant
--FILE--
<?php
use Encoding\Base32;
use function Encoding\base32_encode;

echo base32_encode('Hello world!', variant: Base32::Ascii);
echo "\n";
?>
--EXPECT--
JBSWY3DPEBLW64TMMQ======
