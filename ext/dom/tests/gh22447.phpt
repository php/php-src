--TEST--
GH-22447 (UAF at dom_objects_free_storage when setAttributeNode collides with a namespaced attribute of the same local name)
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();

$attribute1 = $dom->createAttribute("my-attribute");
$container  = $dom->appendChild($dom->createElement("container"));
$attribute2 = $dom->createAttribute("my-attribute");
$attribute4 = $dom->createAttributeNS("urn:a", "my-attribute");

$container->setAttributeNode($attribute1);
$container->setAttributeNode($attribute4);

var_dump($container->setAttributeNode($attribute2) === $attribute1);
var_dump($container->setAttributeNode($attribute1) === $attribute2);

echo $dom->saveXml($container), PHP_EOL;
?>
--EXPECT--
bool(true)
bool(true)
<container xmlns="http://www.w3.org/1999/xhtml" xmlns:ns1="urn:a" ns1:my-attribute="" my-attribute=""></container>
