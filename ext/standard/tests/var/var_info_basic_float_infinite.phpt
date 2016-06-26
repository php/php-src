--TEST--
var_info() correctly identifies infinite floats
--FILE--
<?php

($type = var_info(-INF)) === 'infinite float' || var_dump($type);
($type = var_info( INF)) === 'infinite float' || var_dump($type);

?>
--EXPECT--
