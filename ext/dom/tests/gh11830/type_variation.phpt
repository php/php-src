--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - type variation
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <test/>
    <child><testelement/></child>
</container>
XML);

$testElement = $doc->documentElement->firstElementChild->nextElementSibling->firstElementChild;

try {
    $doc->documentElement->firstElementChild->prepend($testElement, 0);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->append($testElement, true);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->before($testElement, null);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->after($testElement, new stdClass);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->replaceWith($testElement, []);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo $doc->saveXML();
?>
--EXPECT--
DOMElement::prepend(): Argument #2 must be of type DOMNode|string, int given
DOMElement::append(): Argument #2 must be of type DOMNode|string, bool given
DOMElement::before(): Argument #2 must be of type DOMNode|string, null given
DOMElement::after(): Argument #2 must be of type DOMNode|string, stdClass given
DOMElement::replaceWith(): Argument #2 must be of type DOMNode|string, array given
<?xml version="1.0"?>
<container>
    <test/>
    <child><testelement/></child>
</container>
