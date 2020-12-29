--TEST--
JsonParser::isValid() test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = file_get_contents(__DIR__ . DIRECTORY_SEPARATOR . "_files" . DIRECTORY_SEPARATOR . "result.json");
$value = JsonParser::isValid($json);
var_dump($value);

$value = JsonParser::isValid("{"corrupt": true,");
var_dump($value);

$value = JsonParser::isValid("{"corrupt" true}");
var_dump($value);

$value = JsonParser::isValid("Invalid JSON string");
var_dump($value);

?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
bool(false)
