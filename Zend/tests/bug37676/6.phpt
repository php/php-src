--TEST--
Bug #37676 (Complex warnings)
--FILE--
<?php
$a = [0 => null];
$b = [1 => 0];
$c = [2 => 1];
$d = [3 => $b];

$a[$b[$c[2]]][0];
$a[$d[3][1]][$b[1]];
?>
--EXPECTF--
Warning: Variable of type null does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d
