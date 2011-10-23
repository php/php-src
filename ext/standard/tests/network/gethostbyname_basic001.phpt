--TEST--
gethostbyname() function - basic type return test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
	var_dump(is_string(gethostbyname("www.php.net")));
?>
--EXPECT--
bool(true)
