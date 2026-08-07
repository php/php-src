--TEST--
Base58 encoding default Bitcoin variant
--FILE--
<?php
use function Encoding\base58_encode;

echo base58_encode('Hello world!');
echo "\n";
?>
--EXPECT--
72k1xXWG59fYdzSNoA
