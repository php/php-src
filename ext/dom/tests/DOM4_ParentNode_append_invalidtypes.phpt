--TEST--
DOMParentNode::append() exception on invalid argument
--EXTENSIONS--
dom
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test />');

try {
    $dom->documentElement->append(array());
} catch(TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: DOMElement::append(): Argument #1 must be of type DOMNode|string, array given
