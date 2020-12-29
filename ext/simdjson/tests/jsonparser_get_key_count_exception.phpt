--TEST--
JsonParser::getKeyCount() throws exception if key was not found test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = file_get_contents(__DIR__ . DIRECTORY_SEPARATOR . "_files" . DIRECTORY_SEPARATOR . "result.json");

try {
    JsonParser::getKeyCount($json, "unknown", true);
} catch (JsonException $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(67) "The JSON document was too deep (too many nested objects and arrays)"
