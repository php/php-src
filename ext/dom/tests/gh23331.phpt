--TEST--
GH-23331 (Use-after-free when an attribute child past an entity reference keeps a live wrapper)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<!DOCTYPE root [<!ENTITY e "X">]><root attr="a&e;b"/>');
$attr = $doc->documentElement->getAttributeNode('attr');
$first = $attr->firstChild;
$entity = $attr->childNodes[1];
$last = $attr->lastChild;

$doc->documentElement->setAttribute('attr', 'updated');

echo "text before the entity reference: ";
var_dump($first->textContent);
echo "entity reference name: ";
var_dump($entity->nodeName);
echo "entity reference detached: ";
var_dump($entity->parentNode === null);
echo "text after the entity reference: ";
var_dump($last->textContent);
echo "detached from the attribute: ";
var_dump($last->parentNode === null);
echo "new attribute value: ";
var_dump($doc->documentElement->getAttribute('attr'));

?>
--EXPECT--
text before the entity reference: string(1) "a"
entity reference name: string(1) "e"
entity reference detached: bool(true)
text after the entity reference: string(1) "b"
detached from the attribute: bool(true)
new attribute value: string(7) "updated"
