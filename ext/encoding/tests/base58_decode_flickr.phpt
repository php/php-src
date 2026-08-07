--TEST--
Base58 decode Flickr data with Flickr variant
--FILE--
<?php
use Encoding\Base58;
use function Encoding\base58_decode;

var_dump(base58_decode("Z7Pznk19XTTzBtx", variant: Base58::Flickr));
?>
--EXPECT--
string(12) "Hello world!"
