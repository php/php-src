--TEST--
Base58 decode Bitcoin data
--FILE--
<?php
use function Encoding\base58_decode;

var_dump(base58_decode("72k1xXWG59fYdzSNoA"));
?>
--EXPECT--
string(12) "Hello world!"
