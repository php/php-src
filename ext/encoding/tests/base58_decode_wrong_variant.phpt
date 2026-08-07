--TEST--
Base58 decode Flickr data with default Bitcoin variant
--FILE--
<?php
use function Encoding\base58_decode;

$encodedFlickr = "Z7Pznk19XTTzBtx";

try {
    $result = base58_decode($encodedFlickr);
    // Flickr data decoded with Bitcoin alphabet - may return meaningless data
    // or throw exception if unsupported characters found
    echo "Result: $result\n";
    echo "Length: " . strlen($result) . "\n";
} catch (Encoding\UnableToDecodeException $e) {
    echo "UnableToDecodeException caught\n";
}
?>
--EXPECTF--
Result: %s
Length: %d
