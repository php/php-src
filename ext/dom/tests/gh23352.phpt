--TEST--
GH-23352 (UAF reading an attribute value node retained across DOMDocument::adoptNode())
--EXTENSIONS--
dom
--FILE--
<?php

$source = new DOMDocument();
$element = $source->appendChild($source->createElement('element'));
$element->setAttribute('attribute', 'victim');
$victim = $element->getAttributeNode('attribute')->firstChild;

$destination = new DOMDocument();
$destination->appendChild($destination->adoptNode($element));
unset($destination, $source, $element);

echo $victim->data, PHP_EOL;

?>
--EXPECT--
victim
