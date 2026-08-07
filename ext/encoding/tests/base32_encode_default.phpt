--TEST--
Base32 encoding default (ASCII with padding)
--FILE--
<?php
use function Encoding\base32_encode;

echo base32_encode('Hello world!');
echo "\n";
?>
--EXPECT--
JBSWY3DPEB3W64TMMQQQ====
