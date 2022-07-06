--TEST--
DOMParentNode::append() with DOMNode from wrong document throws exception
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom1 = new DOMDocument;
$dom1->loadXML('<test/>');

$dom2 = new DOMDocument;
$dom2->loadXML('<test><foo /></test>');

$element = $dom1->documentElement;

try {
    $element->append($dom2->documentElement->firstChild);
    echo "FAIL";
} catch (DOMException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $element->prepend($dom2->documentElement->firstChild);
    echo "FAIL";
} catch (DOMException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Wrong Document Error
Wrong Document Error
