--TEST--
GH-23116 (Stack overflow when normalizing a deeply nested DOMDocument)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();

$node = $doc->createElement('leaf');
$node->appendChild($doc->createTextNode('x'));
$node->appendChild($doc->createTextNode(''));
$node->appendChild($doc->createTextNode('y'));
$leaf = $node;

for ($i = 0; $i < 100000; $i++) {
    $parent = $doc->createElement('a');
    $parent->appendChild($node);
    $node = $parent;
}

$doc->appendChild($node);
$doc->normalize();

var_dump($leaf->childNodes->length);
var_dump($leaf->textContent);
?>
--EXPECT--
int(1)
string(2) "xy"
