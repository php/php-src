--TEST--
Dom\Node::appendChild() with attribute should fail
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();

$element = $dom->createElement('foo');
$attr = $dom->createAttribute('bar');
$attr->value = "hello";
try {
    $element->appendChild($attr);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($attr->value);

?>
--EXPECT--
DOMException: Hierarchy Request Error
string(5) "hello"
