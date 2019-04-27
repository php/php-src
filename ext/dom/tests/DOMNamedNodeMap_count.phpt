--TEST--
Test count nodes in DOMNamedNodeMap
--CREDITS--
Andreas Treichel <gmblar+github@gmail.com>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$document = new DomDocument();
$root = $document->createElement('root');
$document->appendChild($root);
for($i = 0; $i < 5; $i++) {
    $root->setAttribute('attribute-' . $i, 'value-' . $i);
}
for($i = 0; $i < 7; $i++) {
    $item = $document->createElement('item');
    $root->appendChild($item);
}
var_dump($root->attributes->length);
var_dump($root->attributes->count());
var_dump(count($root->attributes));

?>
--EXPECT--
int(5)
int(5)
int(5)
