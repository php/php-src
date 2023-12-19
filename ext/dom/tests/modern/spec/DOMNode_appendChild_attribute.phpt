--TEST--
DOMNode::appendChild() with attribute should fail
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();

$element = $dom->createElement('foo');
$attr = $dom->createAttribute('bar');
$attr->value = "hello";
try {
    $element->appendChild($attr);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

var_dump($attr->value);

?>
--EXPECT--
Hierarchy Request Error
string(5) "hello"
