--TEST--
http_build_query() function with null in array
--FILE--
<?php
var_dump(http_build_query([null]));
?>
--EXPECT--
string(0) ""
