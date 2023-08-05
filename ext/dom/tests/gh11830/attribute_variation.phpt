--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - attribute variation
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container x="foo">
    <test/>
</container>
XML);

try {
    $doc->documentElement->firstElementChild->prepend($doc->documentElement->attributes[0]);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->append($doc->documentElement->attributes[0]);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->before($doc->documentElement->attributes[0]);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->after($doc->documentElement->attributes[0]);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->replaceWith($doc->documentElement->attributes[0]);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $doc->saveXML();
?>
--EXPECTF--
Hierarchy Request Error

Fatal error: Uncaught TypeError: DOMElement::append(): Argument #1 must be of type DOMNode|string, null given in %s:%d
Stack trace:
#0 %s(%d): DOMElement->append(NULL)
#1 {main}
  thrown in %s on line %d
