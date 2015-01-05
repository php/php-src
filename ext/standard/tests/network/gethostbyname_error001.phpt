--TEST--
gethostbyname() function - basic type return error test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
	var_dump(is_string(gethostbyname("192.168.0.101")));
?>
--EXPECT--
bool(true)
