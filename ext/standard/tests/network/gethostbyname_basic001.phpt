--TEST--
gethostbyname() function - basic type return test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
	var_dump(is_string(gethostbyname("www.php.net")));
?>
--EXPECT--
bool(true)
