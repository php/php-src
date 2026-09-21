--TEST--
setAttributeNS() keeps an attribute child that still has a live wrapper
--EXTENSIONS--
dom
--FILE--
<?php

$doc = Dom\XMLDocument::createFromString('<root xmlns:p="urn:x" p:attr="old"/>');
$el = $doc->documentElement;
$text = $el->getAttributeNodeNS('urn:x', 'attr')->firstChild;
$el->setAttributeNS('urn:x', 'p:attr', 'new');
echo "prefixed, detached: ";
var_dump($text->parentNode === null);
echo "prefixed, text: ";
var_dump($text->textContent);
echo "prefixed, new value: ";
var_dump($el->getAttributeNS('urn:x', 'attr'));

$doc = Dom\XMLDocument::createFromString('<root attr="old"/>');
$el = $doc->documentElement;
$text = $el->getAttributeNode('attr')->firstChild;
$el->setAttributeNS(null, 'attr', 'new');
echo "no namespace, detached: ";
var_dump($text->parentNode === null);
echo "no namespace, text: ";
var_dump($text->textContent);
echo "no namespace, new value: ";
var_dump($el->getAttribute('attr'));

?>
--EXPECT--
prefixed, detached: bool(true)
prefixed, text: string(3) "old"
prefixed, new value: string(3) "new"
no namespace, detached: bool(true)
no namespace, text: string(3) "old"
no namespace, new value: string(3) "new"
