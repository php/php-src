--TEST--
getprotobynumber function basic test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
	var_dump(getprotobynumber(6));
?>
--EXPECT--
string(3) "tcp"
