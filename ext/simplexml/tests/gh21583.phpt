--TEST--
GH-21583 (empty SimpleXML child element casts to false)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = simplexml_load_string(<<<XML
<root>
    <empty/>
    <with-text>content</with-text>
    <with-attribute value="1"/>
    <with-child><child/></with-child>
</root>
XML);

var_dump((bool) $xml->empty);
var_dump((bool) $xml->missing);
var_dump((bool) $xml->{"with-text"});
var_dump((bool) $xml->{"with-attribute"});
var_dump((bool) $xml->{"with-child"});
var_dump((bool) simplexml_load_string('<root/>'));

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
