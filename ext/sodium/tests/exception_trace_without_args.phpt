--TEST--
SodiumException backtraces do not contain function arguments
--SKIPIF--
<?php if (!extension_loaded("sodium")) die("skip"); ?>
--FILE--
<?php

function do_memzero($x) {
	sodium_memzero($x);
}

$x = 42;
do_memzero($x);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: sodium_memzero() expects parameter 1 to be string, integer given in %s:%d
Stack trace:
#0 %s(%d): sodium_memzero()
#1 %s(%d): do_memzero()
#2 {main}
  thrown in %s on line %d
