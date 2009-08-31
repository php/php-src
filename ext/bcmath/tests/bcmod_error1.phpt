--TEST--
bcmod — Get modulus of an arbitrary precision number
--CREDITS--
Antoni Torrents
antoni@solucionsinternet.com
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bcmod('1', '2', '3');
?>
--EXPECTF--
Warning: Wrong parameter count for bcmod() in %s.php on line %d
