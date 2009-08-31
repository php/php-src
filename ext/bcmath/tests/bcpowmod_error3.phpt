--TEST--
bcpowmod — Raise an arbitrary precision number to another, reduced by a specified modulus
--CREDITS--
Antoni Torrents
antoni@solucionsinternet.com
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bcpowmod('1', '2', '3', '4', '5');
?>
--EXPECTF--
Warning: bcpowmod() expects at most 4 parameters, 5 given in %s.php on line %d
