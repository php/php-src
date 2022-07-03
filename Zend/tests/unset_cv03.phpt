--TEST--
unset() CV 3 (unset() global variable in included file)
--FILE--
<?php
$x = "ok\n";
echo $x;
include "unset.inc";
echo $x;
?>
--EXPECTF--
ok

Warning: Undefined variable $x (This will become an error in PHP 9.0) in %s on line %d
