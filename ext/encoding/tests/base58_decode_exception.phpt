--TEST--
Base58 decode wrong variant exception
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\UnableToDecodeException;

use function Encoding\base58_decode;

$encodedFlickr = "2nePN7syqGMz7VrVk";

try {
    base58_decode($encodedFlickr);
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

?>
--EXPECTF--
UnableToDecodeException
