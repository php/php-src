--TEST--
bcdiv — Divide two arbitrary precision numbers
--CREDITS--
Antoni Torrents
antoni@solucionsinternet.com
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bcdiv('1', '2', '3', '4');
?>
--EXPECTF--
Warning: Wrong parameter count for bcdiv() in %s.php on line %d
