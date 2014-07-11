--TEST--
bcsqrt — Get the square root of an arbitrary precision number
--CREDITS--
Antoni Torrents
antoni@solucionsinternet.com
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bcsqrt('-9');
?>
--EXPECTF--
Warning: bcsqrt(): Square root of negative number in %s.php on line %d
