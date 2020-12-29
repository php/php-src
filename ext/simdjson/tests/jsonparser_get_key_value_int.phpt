--TEST--
JsonParser::getKeyValue() with int test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = file_get_contents(__DIR__ . DIRECTORY_SEPARATOR . "_files" . DIRECTORY_SEPARATOR . "result.json");
$value = JsonParser::getKeyValue($json, "code", false);
var_dump($value);

$value = JsonParser::getKeyValue($json, "code", true);
var_dump($value);

?>
--EXPECT--
int(201)
int(201)
