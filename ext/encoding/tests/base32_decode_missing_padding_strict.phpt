--TEST--
Base32 decode missing padding strict throws UnableToDecodeException
--FILE--
<?php
use function Encoding\base32_decode;

try {
    base32_decode("JBSWY3DPEBLW64TMMQ");
    echo "No exception thrown\n";
} catch (Encoding\UnableToDecodeException $e) {
    echo "UnableToDecodeException caught\n";
}
?>
--EXPECT--
UnableToDecodeException caught
