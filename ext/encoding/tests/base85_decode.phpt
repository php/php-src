--TEST--
Base85 decode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base85;

use function Encoding\base85_decode;

$encodedZ85 = "nm=QNzY<mxA+]nf";

var_dump(base85_decode($encodedZ85, variant: Base85::Z85));

?>
--EXPECT--
string(12) "Hello world!"
