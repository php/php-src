--TEST--
SimpleXMLElement reconstruction with a retained child
--EXTENSIONS--
simplexml
--FILE--
<?php
$xml = simplexml_load_string('<root><child>old</child></root>');
$child = $xml->child;

$xml->__construct('<root><new>new</new></root>');

var_dump((string) $xml->new);
var_dump((string) $child);
var_dump($child->asXML());
?>
--EXPECT--
string(3) "new"
string(3) "old"
string(18) "<child>old</child>"
