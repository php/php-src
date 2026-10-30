--TEST--
Import of lone attribute
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement('root'));

$root->setAttributeNS("urn:a", "a:foo", "bar");
$attr = $root->getAttributeNodeNS("urn:a", "foo");

$importer = Dom\XMLDocument::createEmpty();
$attr = $importer->importNode($attr, true);
echo $importer->saveXml($attr), "\n";

unset($dom);
unset($root);

var_dump($attr->prefix, $attr->namespaceURI);

?>
--EXPECT--
a:foo="bar"
string(1) "a"
string(5) "urn:a"
