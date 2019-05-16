--TEST--
Bug #61115: Stream related segfault on fatal error in php_stream_context_del_link.
--FILE--
<?php

$arrayLarge = array_fill(0, 113663, '*');

$resourceFileTemp = fopen('php://temp', 'r+');
stream_context_set_params($resourceFileTemp, array());
preg_replace('', function() {}, $resourceFileTemp);
?>
--EXPECTF--
Recoverable fatal error: Object of class Closure could not be converted to string in %s on line %d
