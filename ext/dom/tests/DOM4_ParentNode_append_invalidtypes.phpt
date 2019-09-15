--TEST--
DOMParentNode::append() exception on invalid argument
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test />');
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

try {
    $dom->documentElement->append(array());
} catch(TypeError $e) {
    echo "OK!";
}
--EXPECT--
OK!
