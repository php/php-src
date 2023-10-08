--TEST--
Delayed freeing element declaration
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<'XML'
<?xml version="1.0"?>
<!DOCTYPE books [
<!ELEMENT parent (child1, child2)>
<!ELEMENT child1 (#PCDATA)>
<!ELEMENT child2 (#PCDATA)>
]>
<container><parent/></container>
XML, LIBXML_NOENT);
$element = $doc->documentElement->firstElementChild;
echo $doc->saveXML(), "\n";
var_dump($element->tagName);
var_dump($element->textContent);

$doc->removeChild($doc->doctype);
echo $doc->saveXML(), "\n";
var_dump($element->tagName);
var_dump($element->textContent);
?>
--EXPECT--
<?xml version="1.0"?>
<!DOCTYPE books [
<!ELEMENT parent (child1 , child2)>
<!ELEMENT child1 (#PCDATA)>
<!ELEMENT child2 (#PCDATA)>
]>
<container><parent/></container>

string(6) "parent"
string(0) ""
<?xml version="1.0"?>
<container><parent/></container>

string(6) "parent"
string(0) ""
