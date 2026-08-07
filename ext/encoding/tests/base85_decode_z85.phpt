--TEST--
Base85 decode Z85 data with Z85 variant
--FILE--
<?php
use Encoding\Base85;
use function Encoding\base85_decode;

var_dump(base85_decode("nm=QNz.92Pz/P", variant: Base85::Z85));
?>
--EXPECT--
string(12) "Hello world!"
