--TEST--
DOM\Element::setAttributeNode(NS) inuse error
--EXTENSIONS--
dom
--FILE--
<?php

$dom1 = DOM\HTMLDocument::createEmpty();
$container = $dom1->appendChild($dom1->createElement("container"));
$attr1 = $dom1->createAttribute("my-attribute");
$attr1->value = "1";
$container->setAttributeNode($attr1);
$element = $container->appendChild($dom1->createElement("element"));
try {
    $element->setAttributeNode($attr1);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom1->saveHTML(), "\n";

?>
--EXPECT--
Inuse Attribute Error
<container my-attribute="1"><element></element></container>
