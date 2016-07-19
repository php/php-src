--TEST--
var_info() correctly identifies invalid floats
--FILE--
<?php

($type = var_info(-NAN)) === 'invalid float' || var_dump($type);
($type = var_info( NAN)) === 'invalid float' || var_dump($type);

?>
--EXPECT--
