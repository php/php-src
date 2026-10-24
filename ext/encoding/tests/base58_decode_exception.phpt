--TEST--
Base58 decode invalid character exception
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\UnableToDecodeException;
use function Encoding\base58_decode;

// '0' is not a valid base58 character (excluded to avoid confusion with 'O')
try {
    base58_decode("0");
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

// 'I' is not a valid base58 character
try {
    base58_decode("I");
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

// 'O' is not a valid base58 character
try {
    base58_decode("O");
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

?>
--EXPECTF--
Encoding\UnableToDecodeException
Encoding\UnableToDecodeException
Encoding\UnableToDecodeException
