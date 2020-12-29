--TEST--
JsonParser::getKeyValue() throws exception if key was not found test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = file_get_contents(__DIR__ . DIRECTORY_SEPARATOR . "_files" . DIRECTORY_SEPARATOR . "result.json");

try {
    JsonParser::getKeyValue($json, "unknown", true);
} catch (JsonException $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(56) "The JSON field referenced does not exist in this object."
