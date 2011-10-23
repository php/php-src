--TEST--
gethostbyname() function - basic return valid ip address test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
	$ip = gethostbyname("www.php.net");
	var_dump((bool) ip2long($ip));
?>
--EXPECT--
bool(true)
