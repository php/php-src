--TEST--
sqlite: crash inside sqlite_escape_string() & sqlite_udf_encode_binary
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--INI--
memory_limit=-1
--FILE--
<?php
	var_dump(strlen(sqlite_escape_string(str_repeat("\0", 20000000))));
	var_dump(strlen(sqlite_udf_encode_binary(str_repeat("\0", 20000000))));
?>
--EXPECT--
int(20000002)
int(20000002)
