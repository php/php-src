--TEST--
GH-15910 (Assertion failure in ext/dom/element.c)
--EXTENSIONS--
dom
--CREDITS--
YuanchengJiang
--FILE--
<?php
$doc = new DOMDocument();
$doc->appendChild($doc->createElement('container'));
try {
    $doc->documentElement->setAttributeNodeNS($doc);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: DOMElement::setAttributeNodeNS(): Argument #1 ($attr) must be of type DOMAttr, DOMDocument given
