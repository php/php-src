--TEST--
http_build_query() function with reference in array
--FILE--
<?php
$v = 'value';
$ref = &$v;

var_dump(http_build_query([$ref]));
?>
--EXPECT--
string(7) "0=value"
