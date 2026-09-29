--TEST--
DOMDocument::adoptNode() with a node retained under a later sibling
--EXTENSIONS--
dom
--FILE--
<?php

$source = new DOMDocument();
$root = $source->appendChild($source->createElement('root'));
$root->appendChild($source->createElement('first'));
$second = $root->appendChild($source->createElement('second'));
$victim = $second->appendChild($source->createElement('grandchild'));

$destination = new DOMDocument();
$destination->appendChild($destination->adoptNode($root));
unset($destination, $source, $root, $second);

echo $victim->nodeName, PHP_EOL;

?>
--EXPECT--
grandchild
