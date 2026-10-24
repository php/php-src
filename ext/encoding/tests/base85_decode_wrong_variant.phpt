--TEST--
Base85 decode wrong variant exception
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base85;
use Encoding\UnableToDecodeException;

use function Encoding\base85_decode;

$encodedZ85 = "nm=QNzY<mxA+]nf";

try {
    base85_decode($encodedZ85, variant: Base85::Adobe);
    echo "No exception\n";
} catch (UnableToDecodeException $e) {
    echo $e::class, "\n";
}

?>
--EXPECTF--
Encoding\UnableToDecodeException
