--TEST--
Delayed freeing attribute declaration
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<'XML'
<?xml version="1.0"?>
<!DOCTYPE book [
<!ELEMENT book (#PCDATA)>
<!ATTLIST book
title CDATA #REQUIRED
>
]>
<book title="book title"/>
XML);
echo $doc->saveXML(), "\n";
// Note: no way to query the attribute declaration, but this at least tests destruction order
$doc->removeChild($doc->doctype);
echo $doc->saveXML(), "\n";
?>
--EXPECT--
<?xml version="1.0"?>
<!DOCTYPE book [
<!ELEMENT book (#PCDATA)>
<!ATTLIST book title CDATA #REQUIRED>
]>
<book title="book title"/>

<?xml version="1.0"?>
<book title="book title"/>
