--TEST--
Bug #67592 (json_encode() option JSON_BIGINT_AS_STRING)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_encode(['a'=>974110684307885632], JSON_BIGINT_AS_STRING));
var_dump(json_encode(['a'=>974110684307885632]));

?>
--EXPECT--
string(26) "{"a":"974110684307885632"}"
string(24) "{"a":974110684307885632}"