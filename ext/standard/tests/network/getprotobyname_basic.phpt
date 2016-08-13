--TEST--
getprotobyname function basic test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
	var_dump(getprotobyname('tcp'));
?>
--EXPECT--
int(6)
