--TEST--
Base16 decode lowercase strict throws UnableToDecodeException
--FILE--
<?php
use function Encoding\base16_decode;

try {
    base16_decode("48656c6c6f20776f726c6421");
    echo "No exception thrown\n";
} catch (Encoding\UnableToDecodeException $e) {
    echo "UnableToDecodeException caught\n";
}
?>
--EXPECT--
UnableToDecodeException caught
