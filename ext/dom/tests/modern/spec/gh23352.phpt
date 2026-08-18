--TEST--
GH-23352 (UAF reading an attribute value node retained across Dom\Document::adoptNode())
--EXTENSIONS--
dom
--FILE--
<?php

$source = Dom\XMLDocument::createFromString('<root/>');
$element = $source->documentElement;
$element->setAttribute('attribute', 'victim');
$victim = $element->getAttributeNode('attribute')->firstChild;

$destination = Dom\XMLDocument::createEmpty();
$destination->appendChild($destination->adoptNode($element));
unset($destination, $source, $element);

echo $victim->data, PHP_EOL;

?>
--EXPECT--
victim
