--TEST--
Test count nodes in DOMNodeList
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
var_dump($root->childNodes->length);
var_dump($root->childNodes->count());
var_dump(count($root->childNodes));

?>
--EXPECT--
int(7)
int(7)
int(7)
