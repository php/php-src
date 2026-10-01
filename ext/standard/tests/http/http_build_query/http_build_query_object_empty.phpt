--TEST--
http_build_query() function with empty object
--FILE--
<?php
class EmptyObj {}
$o = new EmptyObj();

var_dump(http_build_query($o));
?>
--EXPECTF--
Deprecated: http_build_query(): Passing an object for argument #1 $data to http_build_query() is deprecated, call get_object_vars() first instead in %s on line %d
string(0) ""
