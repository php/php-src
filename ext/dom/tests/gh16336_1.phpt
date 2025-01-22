--TEST--
GH-16336 (Attribute intern document mismanagement)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$elem = new DOMElement("g");
$attr = new DOMAttr("iF", "j");

// First append, then attribute
$doc->appendChild($elem);
$elem->setAttributeNode($attr);
echo $attr->firstChild->textContent;

?>
--EXPECT--
j
