--TEST--
DOM\HTML5Document serialization of an attribute in a namespace
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$root = $dom->appendChild($dom->createElement("root"));
$root->setAttributeNodeNS($dom->createAttributeNS("http://php.net", "x:foo"));
$root->setAttributeNodeNS($dom->createAttributeNS("http://www.w3.org/XML/1998/namespace", "y:id"));
// Can't test the following because its behaviour is broken in combination with the live spec
//$root->setAttributeNodeNS($dom->createAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns"));
$root->setAttributeNodeNS($dom->createAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:f"));
$root->setAttributeNodeNS($dom->createAttributeNS("http://www.w3.org/1999/xlink", "z:f"));

// Note: XML declarations are not emitted in HTML5
echo $dom->saveHTML();

?>
--EXPECT--
<root x:foo="" xml:id="" xmlns:f="" xlink:f=""></root>
