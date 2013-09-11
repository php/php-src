--TEST--
file_get_contents() test using offset parameter out of range
--CREDITS--
"Blanche V.N." <valerie_nare@yahoo.fr>
"Sylvain R." <sracine@phpquebec.org>
--INI--
display_errors=false
--SKIPIF--
<?php
	if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
	if (!function_exists("file_get_contents"))
		die ("skip file_get_contents function is not found");
?>
--FILE--
<?php
	var_dump(file_get_contents("http://checkip.dyndns.com",null,null,8000,1));
?>
--EXPECT--
bool(false)
