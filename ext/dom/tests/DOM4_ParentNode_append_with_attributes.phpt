--TEST--
DOMParentNode::append() with attributes
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test attr-one="21"/>');
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$replacement = $dom->createAttribute('attr-one');
$replacement->value ='42';
$addition = $dom->createAttribute('attr-two');
$addition->value = '23';

$element = $dom->documentElement;

try {
    $element->append($replacement, $addition);
} catch (DOMException $e) {
    echo $e->getMessage();
}
--EXPECT--
Hierarchy Request Error
