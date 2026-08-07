--TEST--
Base32 decode ASCII data with Crockford variant
--FILE--
<?php
use Encoding\Base32;
use function Encoding\base32_decode;

$data = 'Hello world!';
$encodedAscii = "JBSWY3DPEBLW64TMMQ======";

try {
    $result = base32_decode($encodedAscii, variant: Base32::Crockford);
    // The ASCII encoded data contains '=' which is not in Crockford alphabet
    // This should throw UnableToDecodeException
    echo "Result: $result\n";
} catch (Encoding\UnableToDecodeException $e) {
    echo "UnableToDecodeException caught\n";
}
?>
--EXPECT--
UnableToDecodeException caught
