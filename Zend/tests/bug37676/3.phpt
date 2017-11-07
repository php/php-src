--TEST--
Bug #37676 (Argument warnings)
--FILE--
<?php
function f ($a) {}

$a = false;
f($a[0][1][2]);

$a = 1;
f($a[0][1][2]);

$a = null;
f($a[0][1][2]);
?>
--EXPECTF--
Warning: Variable of type bool does not accept array offsets in %s on line %d

Warning: Variable of type int does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d
