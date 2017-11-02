--TEST--
Bug #37676 (Simple warnings)
--FILE--
<?php
$a = false;
$a[0];
@$a[0];
isset($a[0]);

$a = 1;
$a[0];
@$a[0];
isset($a[0]);

$a = null;
$a[0];
@$a[0];
isset($a[0]);

$a = 1;
$b = 2;
$a[0] + $b[0];
?>
--EXPECTF--
Warning: Variable of type boolean does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d
