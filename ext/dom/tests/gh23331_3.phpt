--TEST--
GH-23331 (Use-after-free when an attribute child past an entity reference keeps a live wrapper) - Dom\XMLDocument
--EXTENSIONS--
dom
--FILE--
<?php

$xml = '<!DOCTYPE root [<!ENTITY e "X">]><root xmlns:p="urn:x" attr="a&e;b" p:nsattr="c&e;d"/>';

$doc = Dom\XMLDocument::createFromString($xml);
$el = $doc->documentElement;
$attr = $el->getAttributeNode('attr');
$first = $attr->firstChild;
$last = $attr->lastChild;
unset($attr);
$el->removeAttribute('attr');
echo "removeAttribute, first: ";
var_dump($first->textContent);
echo "removeAttribute, detached: ";
var_dump($last->parentNode === null);
echo "removeAttribute, text: ";
var_dump($last->textContent);

$doc = Dom\XMLDocument::createFromString($xml);
$el = $doc->documentElement;
$attr = $el->getAttributeNodeNS('urn:x', 'nsattr');
$last = $attr->lastChild;
unset($attr);
$el->removeAttributeNS('urn:x', 'nsattr');
echo "removeAttributeNS, detached: ";
var_dump($last->parentNode === null);
echo "removeAttributeNS, text: ";
var_dump($last->textContent);

?>
--EXPECT--
removeAttribute, first: string(1) "a"
removeAttribute, detached: bool(true)
removeAttribute, text: string(1) "b"
removeAttributeNS, detached: bool(true)
removeAttributeNS, text: string(1) "d"
