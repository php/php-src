--TEST--
unset() CV 1 (unset() global variable)
--FILE--
<?php
$x = "ok\n";
echo $x;
unset($x);
echo $x;
?>
--EXPECTF--
ok

Warning: Undefined variable $x (This will become an error in PHP 9.0) in %s on line %d
