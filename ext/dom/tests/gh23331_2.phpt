--TEST--
GH-23331 (Use-after-free when an attribute child past an entity reference keeps a live wrapper) - setAttributeNS() and removeAttribute()
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<!DOCTYPE root [<!ENTITY e "X">]><root xmlns:p="urn:x" p:attr="a&e;b"/>');
$attr = $doc->documentElement->getAttributeNodeNS('urn:x', 'attr');
$last = $attr->lastChild;
$doc->documentElement->setAttributeNS('urn:x', 'p:attr', 'updated');
echo "setAttributeNS, detached: ";
var_dump($last->parentNode === null);
echo "setAttributeNS, text: ";
var_dump($last->textContent);

$doc = new DOMDocument();
$doc->loadXML('<!DOCTYPE root [<!ENTITY e "X">]><root attr="a&e;b"/>');
$attr = $doc->documentElement->getAttributeNode('attr');
$last = $attr->lastChild;
unset($attr);
$doc->documentElement->removeAttribute('attr');
echo "removeAttribute, no wrapper on the attribute, detached: ";
var_dump($last->parentNode === null);
echo "removeAttribute, no wrapper on the attribute, text: ";
var_dump($last->textContent);

?>
--EXPECT--
setAttributeNS, detached: bool(true)
setAttributeNS, text: string(1) "b"
removeAttribute, no wrapper on the attribute, detached: bool(true)
removeAttribute, no wrapper on the attribute, text: string(1) "b"
