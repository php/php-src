--TEST--
gethostbyname() function - basic type return test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (getenv("SKIP_ONLINE_TESTS")) die("skip test requiring internet connection");
?>
--FILE--
<?php
	var_dump(is_string(gethostbyname("www.php.net")));
?>
--EXPECT--
bool(true)
