--TEST--
Base58 encoding explicit Bitcoin variant
--FILE--
<?php
use Encoding\Base58;
use function Encoding\base58_encode;

echo base58_encode('Hello world!', variant: Base58::Bitcoin);
echo "\n";
?>
--EXPECT--
72k1xXWG59fYdzSNoA
