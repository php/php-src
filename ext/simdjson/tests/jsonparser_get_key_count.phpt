--TEST--
JsonParser::getKeyCount() test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = file_get_contents(__DIR__ . DIRECTORY_SEPARATOR . "_files" . DIRECTORY_SEPARATOR . "result.json");
$value = JsonParser::getKeyCount($json, "result");
var_dump($value);

$value = JsonParser::getKeyCount($json, "extra/stats");
var_dump($value);

?>
--EXPECT--
int(2)
int(8)
