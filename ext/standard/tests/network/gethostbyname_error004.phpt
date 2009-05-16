--TEST--
gethostbyname() function - basic return valid ip address test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
	$ip = gethostbyname("www.php.net");
	var_dump((bool) ip2long($ip));
?>
--EXPECT--
bool(true)
