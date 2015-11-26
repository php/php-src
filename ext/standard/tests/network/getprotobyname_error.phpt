--TEST--
getprotobyname function errors test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
	// empty protocol name
	var_dump(getprotobyname());

	// invalid protocol name
	var_dump(getprotobyname('abc'));
?>
--EXPECTF--
Warning: getprotobyname() expects exactly 1 parameter, 0 given in %s on line %d
NULL
bool(false)
