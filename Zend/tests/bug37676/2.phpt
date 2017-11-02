--TEST--
Bug #37676 (Chained warnings)
--FILE--
<?php
$a = false;
$a[0][1][2];

$a = 1;
$a[0][1][2];

$a = null;
$a[0][1][2];

$a = 1;
$b = 2;
$a[0][1][2] + $b[0][1][2];
?>
--EXPECTF--
Warning: Variable of type boolean does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d
