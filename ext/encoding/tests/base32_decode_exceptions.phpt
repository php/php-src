--TEST--
Base32 decode strict exceptions
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base32;
use Encoding\UnableToDecodeException;

use function Encoding\base32_decode;

try {
    base32_decode("JBSWY3DPEB3W64TMMQQQ");
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

$big = "IJUWO===";
var_dump(base32_decode($big));

try {
    base32_decode($big, variant: Base32::Crockford);
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

?>
--EXPECTF--
Encoding\UnableToDecodeException
string(3) "Big"
Encoding\UnableToDecodeException
