--TEST--
getprotobynumber function basic test
--SKIPIF--
<?php if (!function_exists('getprotobynumber')) die('skip getprotobynumber function not available'); ?>
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
	var_dump(getprotobynumber(6));
?>
--EXPECT--
string(3) "tcp"
