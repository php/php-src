--TEST--
GH-15712: overflow on real number printing
--FILE--
<?php
ini_set('precision', 1100000000);
echo  -1 * (2 ** -10);
?>
--EXPECTF--
%s
