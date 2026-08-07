--TEST--
Base58 encoding Flickr variant
--FILE--
<?php
use Encoding\Base58;
use function Encoding\base58_encode;

echo base58_encode('Hello world!', variant: Base58::Flickr);
echo "\n";
?>
--EXPECT--
Z7Pznk19XTTzBtx
