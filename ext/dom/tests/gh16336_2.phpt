--TEST--
GH-16336 (Attribute intern document mismanagement)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$elem = new DOMElement("g");
$attr = new DOMAttr("iF", "j");

// First attribute, then append
$elem->setAttributeNode($attr);
$doc->appendChild($elem);
echo $attr->firstChild->textContent;

?>
--EXPECT--
j
