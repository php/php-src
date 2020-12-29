--TEST--
JsonParser::parse() throws exception if json is invalid
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

$json = "Invalid JSON string";

try {
    JsonParser::parse($json, true);
} catch (JsonException $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(102) "The JSON document has an improper structure: missing or superfluous commas, braces, missing keys, etc."
