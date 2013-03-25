--TEST--
bcpowmod — Raise an arbitrary precision number to another, reduced by a specified modulus
--CREDITS--
Antoni Torrents
antoni@solucionsinternet.com
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bcpowmod('1', '2');
?>
--EXPECTF--
Warning: bcpowmod() expects at least 3 parameters, 2 given in %s.php on line %d
