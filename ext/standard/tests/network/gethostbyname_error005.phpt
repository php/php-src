--TEST--
gethostbyname() function - basic invalid parameter test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--INI--
display_errors=false
--FILE--
<?php
	var_dump(gethostbyname());
?>
--EXPECT--
NULL
