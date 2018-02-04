--TEST--
getprotobynumber function errors test
--SKIPIF--
<?php if (!function_exists('getprotobynumber')) die('skip getprotobynumber function not available'); ?>
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
	// empty protocol number
	var_dump(getprotobynumber());

	// invalid protocol number
	var_dump(getprotobynumber(999));
?>
--EXPECTF--
Warning: getprotobynumber() expects exactly 1 parameter, 0 given in %s on line %d
NULL
bool(false)
