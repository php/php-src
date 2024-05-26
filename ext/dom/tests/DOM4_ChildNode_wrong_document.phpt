--TEST--
DOMChildNode::after(), before, replaceWith with DOMNode from wrong document throws exception
--EXTENSIONS--
dom
--FILE--
<?php
require_once("dom_test.inc");

function test($method) {
    $dom1 = new DOMDocument;
    $dom1->loadXML('<test/>');

    $dom2 = new DOMDocument;
    $dom2->loadXML('<test><foo /></test>');

    $element = $dom1->documentElement;

    try {
        $element->$method($dom2->documentElement->firstChild);
        echo "FAIL";
    } catch (DOMException $e) {
        echo $e->getMessage() . "\n";
    }
}

test("after");
test("before");
test("replaceWith");

?>
--EXPECT--
Wrong Document Error
Wrong Document Error
Wrong Document Error
