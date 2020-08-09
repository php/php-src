--TEST--
DOMParentNode::append() exception on invalid argument
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test />');

try {
    $dom->documentElement->append(array());
} catch(TypeError $e) {
    echo "OK! {$e->getMessage()}";
}
?>
--EXPECT--
OK! DOMElement::append(): Argument #1 must be of type DOMNode|string, array given
