--TEST--
GH-18301: casting to void is allowed in for’s expression lists
--FILE--
<?php

$count = 0;

#[NoDiscard]
function incCount() {
	global $count;
	$count++;
	return $count;
}

for ( $count = 0, (void)incCount(), incCount(); (void)incCount(), incCount() < 30; incCount(), $count++, incCount(), (void)incCount()) {
	echo $count . "\n";
}

?>
--EXPECTF--
Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d
4

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d
10

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d
16

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d
22

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d
28

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d

Warning: The return value of function incCount() should either be used or intentionally ignored by casting it as (void) in %s on line %d
