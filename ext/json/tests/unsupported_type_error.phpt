--TEST--
An error is thrown when an unsupported type is encoded
--FILE--
<?php

$resource = fopen(__FILE__, "r");

var_dump($resource);

var_dump(json_encode($resource));
var_dump(json_last_error());

var_dump(json_encode($resource, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error());

?>
--EXPECTF--
resource(5) of type (stream)

Warning: json_encode(): type is unsupported in %s on line %d
bool(false)
int(8)

Warning: json_encode(): type is unsupported in %s on line %d
string(4) "null"
int(8)
