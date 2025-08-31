--TEST--
Bug #37076 (SimpleXML ignores .=) (appending to unnamed attribute)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = simplexml_load_string("<root><foo /></root>");

try {
    $xml->{""} .= "bar";
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

print $xml->asXML();
?>
--EXPECT--
Cannot create element with an empty name
<?xml version="1.0"?>
<root><foo/></root>
