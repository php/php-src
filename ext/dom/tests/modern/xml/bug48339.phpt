--TEST--
Bug #48339 (DOMElement::setIdAttribute() throws 'Not Found Error' exception)
--EXTENSIONS--
dom
--FILE--
<?php
$document = DOM\XMLDocument::createEmpty();
$element = $document->createElement('test');
$element = $document->appendChild($element);
$element->setAttribute("xml:id", 1);
$element->setIdAttribute("xml:id", TRUE);
echo $document->saveXML(), "\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<test xml:id="1"/>
