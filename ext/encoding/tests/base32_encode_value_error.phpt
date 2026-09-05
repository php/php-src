--TEST--
Base32 encode ValueError
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base32;
use Encoding\PaddingMode;

use function Encoding\base32_encode;

$data = 'Hello world!';

try {
    base32_encode($data, variant: Base32::Crockford, paddingMode: PaddingMode::PreservePadding);
    echo "No exception\n";
} catch (ValueError $e) {
    echo $e::class, "\n";
}

?>
--EXPECTF--
ValueError
