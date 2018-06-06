--TEST--
Bug #68567 JSON_PARTIAL_OUTPUT_ON_ERROR can result in JSON with null key
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_encode(array("\x80" => 1), JSON_PARTIAL_OUTPUT_ON_ERROR));

?>
===DONE===
--EXPECT--
string(6) "{"":1}"
===DONE===
