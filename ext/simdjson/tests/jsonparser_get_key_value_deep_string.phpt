--TEST--
JsonParser::getKeyValue() with deep string test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = file_get_contents(__DIR__ . DIRECTORY_SEPARATOR . "_files" . DIRECTORY_SEPARATOR . "result.json");
$value = JsonParser::getKeyValue($json, "result/0/Hello3", false);
var_dump($value);

$value = JsonParser::getKeyValue($json, "result/0/Hello3", true);
var_dump($value);

?>
--EXPECT--
string(6) "World3"
string(6) "World3"
