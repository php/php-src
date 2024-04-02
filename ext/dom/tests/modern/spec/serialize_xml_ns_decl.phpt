--TEST--
Document::saveXML() with XML namespace declaration
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root/>');
$root = $dom->documentElement;
$root->setAttributeNS('http://www.w3.org/2000/xmlns/', 'xmlns:xml', 'http://www.w3.org/XML/1998/namespace');
echo $dom->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root/>
