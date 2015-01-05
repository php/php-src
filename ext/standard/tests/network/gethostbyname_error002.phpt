--TEST--
gethostbyname() function - basic type return error test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
	var_dump(is_string(gethostbyname("1234567890")));
?>
--EXPECT--
bool(true)
