--TEST--
Bug #32615 (Replacing and inserting Fragments)
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DomDocument;
$frag = $dom->createDocumentFragment();
$frag->appendChild(new DOMElement('root'));
$dom->appendChild($frag);
$root = $dom->documentElement;

$frag->appendChild(new DOMElement('first'));
$root->appendChild($frag);

$frag->appendChild(new DOMElement('second'));
$root->appendChild($frag);

$node = $dom->createElement('newfirst');
$frag->appendChild($node);
$root->replaceChild($frag, $root->firstChild);

unset($frag);
$frag = $dom->createDocumentFragment();

$frag->appendChild(new DOMElement('newsecond'));
$root->replaceChild($frag, $root->lastChild);

$node = $frag->appendChild(new DOMElement('fourth'));
$root->insertBefore($frag, NULL);

$frag->appendChild(new DOMElement('third'));
$node = $root->insertBefore($frag, $node);

$frag->appendChild(new DOMElement('start'));
$root->insertBefore($frag, $root->firstChild);

$frag->appendChild(new DOMElement('newthird'));
$root->replaceChild($frag, $node);

$frag->appendChild(new DOMElement('newfourth'));
$root->replaceChild($frag, $root->lastChild);

$frag->appendChild(new DOMElement('first'));
$root->replaceChild($frag, $root->firstChild->nextSibling);

$root->removeChild($root->firstChild);

echo $dom->saveXML()."\n";

while ($root->hasChildNodes()) {
   $root->removeChild($root->firstChild);
}

$frag->appendChild(new DOMElement('first'));
$root->insertBefore($frag, $root->firstChild);

$node = $frag->appendChild(new DOMElement('fourth'));
$root->appendChild($frag);

$frag->appendChild(new DOMElement('second'));
$frag->appendChild(new DOMElement('third'));
$root->insertBefore($frag, $node);

echo $dom->saveXML()."\n";

$frag = $dom->createDocumentFragment();
$root = $dom->documentElement;
$root->replaceChild($frag, $root->firstChild);

echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<root><first/><newsecond/><newthird/><newfourth/></root>

<?xml version="1.0"?>
<root><first/><second/><third/><fourth/></root>

<?xml version="1.0"?>
<root><second/><third/><fourth/></root>
