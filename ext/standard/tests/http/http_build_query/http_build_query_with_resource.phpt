--TEST--
http_build_query() function with resource in array
--FILE--
<?php
var_dump(http_build_query([STDIN]));
?>
--EXPECT--
string(0) ""
