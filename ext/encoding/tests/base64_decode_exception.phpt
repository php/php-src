--TEST--
Base64 decode strict exception
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\UnableToDecodeException;

use function Encoding\base64_decode;

try {
    base64_decode("VGhpcyBpcyBhbiBlbmNvZGVkIHN0cmluZw");
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

?>
--EXPECTF--
Encoding\UnableToDecodeException
