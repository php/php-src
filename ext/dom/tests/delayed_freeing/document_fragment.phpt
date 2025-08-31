--TEST--
Delayed freeing document fragment
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$frag = $doc->createDocumentFragment();
$frag->appendChild($doc->createElementNS('some:ns', 'child', 'text content'));
$child = $doc->appendChild($doc->createElement('root'))->appendChild($frag);
var_dump($doc->textContent);
$doc->documentElement->remove();
var_dump($doc->textContent);
unset($doc);
var_dump($child->textContent);

$doc = new DOMDocument;
$doc->appendChild($doc->createElement('container'));
$doc->documentElement->appendChild($doc->importNode($frag));
unset($frag);
var_dump($doc->textContent);

var_dump($child->parentNode);
?>
--EXPECTF--
string(12) "text content"
string(0) ""
string(12) "text content"

Warning: DOMNode::appendChild(): Document Fragment is empty in %s on line %d
string(0) ""
NULL
