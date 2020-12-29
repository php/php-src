--TEST--
JsonParser::keyExists() test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = file_get_contents(__DIR__ . DIRECTORY_SEPARATOR . "_files" . DIRECTORY_SEPARATOR . "result.json");
$value = JsonParser::keyExists($json, "result");
var_dump($value);

$value = JsonParser::keyExists($json, "result/0/Hello3");
var_dump($value);

$value = JsonParser::keyExists($json, "unknown");
var_dump($value);

$value = JsonParser::keyExists($json, "unknown/deep/nested");
var_dump($value);

$value = JsonParser::keyExists($json, "result/10");
var_dump($value);

?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
