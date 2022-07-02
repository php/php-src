--TEST--
DOMChildNode::after(), before, replaceWith with DOMNode from wrong document throws exception
--EXTENSIONS--
dom
--FILE--
<?php
require_once("dom_test.inc");

$dom1 = new DOMDocument;
$dom1->loadXML('<test/>');

$dom2 = new DOMDocument;
$dom2->loadXML('<test><foo /></test>');

$element = $dom1->documentElement;

try {
    $element->after($dom2->documentElement->firstChild);
    echo "FAIL";
} catch (DOMException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $element->before($dom2->documentElement->firstChild);
    echo "FAIL";
} catch (DOMException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $element->replaceWith($dom2->documentElement->firstChild);
    echo "FAIL";
} catch (DOMException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Wrong Document Error
Wrong Document Error
Wrong Document Error
