--TEST--
Base32 encoding Crockford with PreservePadding throws ValueError
--FILE--
<?php
use Encoding\Base32;
use Encoding\PaddingMode;
use function Encoding\base32_encode;

try {
    base32_encode('Hello world!', variant: Base32::Crockford, paddingMode: PaddingMode::PreservePadding);
    echo "No exception thrown\n";
} catch (ValueError $e) {
    echo "ValueError caught\n";
}
?>
--EXPECT--
ValueError caught
