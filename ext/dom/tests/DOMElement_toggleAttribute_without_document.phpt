--TEST--
DOMElement::toggleAttribute() without a document
--EXTENSIONS--
dom
--FILE--
<?php

$element = new DOMElement("container");
$element->toggleAttribute('foo', true);

$dom = new DOMDocument;
$element = $dom->importNode($element, true);
echo $dom->saveXML($element), "\n";

?>
--EXPECT--
<container foo=""/>
