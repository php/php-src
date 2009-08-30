--TEST--
bcdiv — Divide two arbitrary precision numbers
--CREDITS--
TestFest2009
Antoni Torrents
antoni@solucionsinternet.com
--FILE--
<?php
echo bcdiv('10.99', '0');
?>
--EXPECTF--
Warning: bcdiv(): Division by zero in %s.php on line %d
