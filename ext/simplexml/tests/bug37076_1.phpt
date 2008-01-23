--TEST--
Bug #37076 (SimpleXML ignores .=) (appending to unnamed attribute)
--FILE--
<?php
$xml = simplexml_load_string("<root><foo /></root>");
$xml->{""} .= "bar";
print $xml->asXML();
?>
===DONE===
--EXPECT--
<?xml version="1.0"?>
<root><foo/></root>
===DONE===
