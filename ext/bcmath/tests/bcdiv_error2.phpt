--TEST--
bcdiv — Divide two arbitrary precision numbers
--CREDITS--
Antoni Torrents
antoni@solucionsinternet.com
--FILE--
<?php
echo bcdiv('1', '2', '3', '4');
?>
--EXPECTF--
Warning: bcdiv() expects at most 3 parameters, 4 given in %s.php on line %d
