--TEST--
Base32 encoding Crockford variant
--FILE--
<?php
use Encoding\Base32;
use function Encoding\base32_encode;

echo base32_encode('Hello world!', variant: Base32::Crockford);
echo "\n";
?>
--EXPECT--
91JPRV3F41BPYWKCCG
