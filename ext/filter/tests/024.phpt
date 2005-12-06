--TEST--
filter_data() and FS_ENCODED
--GET--

--FILE--
<?php

var_dump(filter_data("\"<br>blah</ph>", FS_ENCODED));
var_dump(filter_data("", FS_ENCODED));
var_dump(filter_data("  text here  ", FS_ENCODED));
var_dump(filter_data("!@#$%^&*()QWERTYUIOP{ASDFGHJKL:\"ZXCVBNM<>?", FS_ENCODED));

echo "Done\n";
?>
--EXPECT--	
string(26) "%22%3Cbr%3Eblah%3C%2Fph%3E"
string(0) ""
string(23) "%20%20text%20here%20%20"
string(74) "%21%40%23%24%25%5E%26%2A%28%29QWERTYUIOP%7BASDFGHJKL%3A%22ZXCVBNM%3C%3E%3F"
Done
