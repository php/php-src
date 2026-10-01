--TEST--
GH-23365 (DOMNode::insertBefore($n, $n) drops the node and leaves a self-referencing sibling list)
--CREDITS--
Alexandre Daubois
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root>text<child/></root>');
$root = $doc->documentElement;

$text = $root->firstChild;
var_dump($root->insertBefore($text, $text) === $text);
var_dump($root->childNodes->length);
var_dump($text->parentNode === $root, $text->nextSibling === $text, $text->previousSibling === $text);

$child = $root->lastChild;
var_dump($root->insertBefore($child, $child) === $child);
var_dump($root->childNodes->length);

echo $doc->saveXML($root), PHP_EOL;

$doc2 = new DOMDocument();
$doc2->loadXML('<root a="1" b="2"/>');
$el = $doc2->documentElement;
$attr = $el->getAttributeNode('a');
var_dump($el->insertBefore($attr, $attr) === $attr);
echo $doc2->saveXML($el), PHP_EOL;

$doc3 = new DOMDocument();
$root3 = $doc3->appendChild($doc3->createElement('root'));
$root3->appendChild($doc3->createTextNode('A'));
$t = $root3->appendChild($doc3->createTextNode('B'));
$root3->insertBefore($t, $t);
$root3->appendChild($t);
echo $doc3->saveXML($root3), PHP_EOL;
unset($t, $root3, $doc3);
echo "done", PHP_EOL;
?>
--EXPECT--
bool(true)
int(2)
bool(true)
bool(false)
bool(false)
bool(true)
int(2)
<root>text<child/></root>
bool(true)
<root a="1" b="2"/>
<root>AB</root>
done
