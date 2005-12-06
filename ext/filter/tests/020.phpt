--TEST--
filter_data() and FS_MAGIC_QUOTES
--GET--

--FILE--
<?php

var_dump(filter_data("test'asd'asd'' asd\'\"asdfasdf", FS_MAGIC_QUOTES));
var_dump(filter_data("'", FS_MAGIC_QUOTES));
var_dump(filter_data("", FS_MAGIC_QUOTES));
var_dump(filter_data(-1, FS_MAGIC_QUOTES));

echo "Done\n";
?>
--EXPECT--	
string(36) "test\'asd\'asd\'\' asd\\\'\"asdfasdf"
string(2) "\'"
string(0) ""
string(2) "-1"
Done
