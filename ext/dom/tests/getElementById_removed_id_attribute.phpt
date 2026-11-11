--TEST--
getElementById() after a removed ID attribute is freed
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<root><a xml:id="x"/><b xml:id="y"/></root>');
$a = $doc->documentElement->firstChild;
$attr = $a->getAttributeNode('xml:id');
$a->removeAttributeNode($attr);
unset($attr);
echo "removed id: ";
var_dump($doc->getElementById('x'));
echo "remaining id: ", $doc->getElementById('y')->nodeName, "\n";

$doc = new DOMDocument();
$doc->loadXML('<!DOCTYPE root [<!ATTLIST e id ID #IMPLIED>]><root><e id="x"/><e id="y"/></root>');
$e = $doc->documentElement->firstChild;
$attr = $e->getAttributeNode('id');
$e->removeAttributeNode($attr);
unset($attr);
echo "removed DTD id: ";
var_dump($doc->getElementById('x'));
echo "remaining DTD id: ", $doc->getElementById('y')->nodeName, "\n";

$doc = Dom\XMLDocument::createFromString('<root><a xml:id="x"/><b xml:id="y"/></root>');
$a = $doc->documentElement->firstChild;
$attr = $a->getAttributeNode('xml:id');
$a->removeAttributeNode($attr);
unset($attr);
echo "removed Dom\\XMLDocument id: ";
var_dump($doc->getElementById('x'));
echo "remaining Dom\\XMLDocument id: ", $doc->getElementById('y')->nodeName, "\n";

$doc = new DOMDocument();
$doc->loadXML('<root><a xml:id="x"/><b xml:id="y"/></root>');
$a = $doc->documentElement->firstChild;
$attr = $a->getAttributeNode('xml:id');
$a->removeAttribute('xml:id');
unset($attr);
echo "removeAttribute() id: ";
var_dump($doc->getElementById('x'));
echo "remaining removeAttribute() id: ", $doc->getElementById('y')->nodeName, "\n";

?>
--EXPECT--
removed id: NULL
remaining id: b
removed DTD id: NULL
remaining DTD id: e
removed Dom\XMLDocument id: NULL
remaining Dom\XMLDocument id: b
removeAttribute() id: NULL
remaining removeAttribute() id: b
