--TEST--
Base58 decode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base58;

use function Encoding\base58_decode;

$encodedBitcoin = "2NEpo7TZRhna7vSvL";
$encodedFlickr = "2nePN7syqGMz7VrVk";

var_dump(base58_decode($encodedBitcoin));

var_dump(base58_decode($encodedFlickr, variant: Base58::Flickr));

?>
--EXPECT--
string(12) "Hello world!"
string(12) "Hello world!"
