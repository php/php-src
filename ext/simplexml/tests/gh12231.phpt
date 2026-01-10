--TEST--
GH-12231 (SimpleXML xpath should warn when returning other return types than node lists)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = "<container><foo/><foo/></container>";
$sxe = simplexml_load_string($xml);

var_dump($sxe->xpath("count(//foo)"));
var_dump($sxe->xpath("string(//foo)"));
var_dump($sxe->xpath("boolean(//foo)"));
var_dump(count($sxe->xpath("//foo")));

?>
--EXPECTF--
Warning: SimpleXMLElement::xpath(): XPath expression must return a node set, number returned in %s on line %d
bool(false)

Warning: SimpleXMLElement::xpath(): XPath expression must return a node set, string returned in %s on line %d
bool(false)

Warning: SimpleXMLElement::xpath(): XPath expression must return a node set, bool returned in %s on line %d
bool(false)
int(2)
