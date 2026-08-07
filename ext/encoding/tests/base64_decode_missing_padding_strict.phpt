--TEST--
Base64 decode missing padding strict throws UnableToDecodeException
--FILE--
<?php
use function Encoding\base64_decode;

try {
    base64_decode("VGhpcyBpcyBhbiBlbmNvZGVkIHN0cmluZw");
    echo "No exception thrown\n";
} catch (Encoding\UnableToDecodeException $e) {
    echo "UnableToDecodeException caught\n";
}
?>
--EXPECT--
UnableToDecodeException caught
