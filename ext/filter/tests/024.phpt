--TEST--
filter_var() and FILTER_SANITIZE_ENCODED
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_var("\"<br>blah</ph>", FILTER_SANITIZE_ENCODED));
var_dump(filter_var("", FILTER_SANITIZE_ENCODED));
var_dump(filter_var("  text here  ", FILTER_SANITIZE_ENCODED));
var_dump(filter_var("!@#$%^&*()QWERTYUIOP{ASDFGHJKL:\"ZXCVBNM<>?", FILTER_SANITIZE_ENCODED));

echo "Done\n";
?>
--EXPECT--	
string(26) "%22%3Cbr%3Eblah%3C%2Fph%3E"
string(0) ""
string(23) "%20%20text%20here%20%20"
string(74) "%21%40%23%24%25%5E%26%2A%28%29QWERTYUIOP%7BASDFGHJKL%3A%22ZXCVBNM%3C%3E%3F"
Done
