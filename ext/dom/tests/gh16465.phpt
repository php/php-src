--TEST--
GH-16465 (Heap buffer overflow in DOMNode->getElementByTagName)
--EXTENSIONS--
dom
--FILE--
<?php

$v10 = new DOMElement("a");
try {
    $v10->getElementsByTagName("text\0something");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $v10->getElementsByTagNameNS("", "text\0something");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $v10->getElementsByTagNameNS("text\0something", "");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMElement::getElementsByTagName(): Argument #1 ($qualifiedName) must not contain any null bytes
DOMElement::getElementsByTagNameNS(): Argument #2 ($localName) must not contain any null bytes
DOMElement::getElementsByTagNameNS(): Argument #1 ($namespace) must not contain any null bytes
