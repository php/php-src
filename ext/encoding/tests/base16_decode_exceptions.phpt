--TEST--
Base16 decode strict exceptions
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base16;
use Encoding\UnableToDecodeException;

use function Encoding\base16_decode;

$encodedLower = "48656c6c6f20776f726c6421";
$encodedWithSpaces = "48 65\n6C\t6C\r6F 20 77 6f 72 6C 64 21";

try {
    base16_decode($encodedLower);
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

try {
    base16_decode($encodedWithSpaces);
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

?>
--EXPECTF--
Encoding\UnableToDecodeException
Encoding\UnableToDecodeException
