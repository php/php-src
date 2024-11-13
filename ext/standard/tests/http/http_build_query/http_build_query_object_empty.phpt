--TEST--
http_build_query() function with empty object
--FILE--
<?php
class EmptyObj {}
$o = new EmptyObj();

var_dump(http_build_query($o));
?>
--EXPECT--
string(0) ""
