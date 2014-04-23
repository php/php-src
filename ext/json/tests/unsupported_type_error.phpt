--TEST--
An error is thrown when an unsupported type is encoded
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$resource = fopen(__FILE__, "r");

var_dump($resource);

var_dump(json_encode($resource));
var_dump(json_last_error(), json_last_error_msg());

var_dump(json_encode($resource, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error(), json_last_error_msg());

?>
--EXPECTF--
resource(%d) of type (stream)
bool(false)
int(8)
string(21) "Type is not supported"
string(4) "null"
int(8)
string(21) "Type is not supported"
