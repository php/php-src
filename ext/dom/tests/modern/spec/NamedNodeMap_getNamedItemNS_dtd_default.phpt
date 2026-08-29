--TEST--
getNamedItemNS() with empty URI must not throw on DTD default attributes
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<XML
<?xml version="1.0"?>
<!DOCTYPE root [
<!ELEMENT root EMPTY>
<!ATTLIST root defaulted CDATA "from-dtd">
]>
<root real="present"/>
XML;

$el = Dom\XMLDocument::createFromString($xml)->documentElement;
$defaulted = $el->attributes->getNamedItemNS('', 'defaulted');
var_dump($defaulted === null ? null : $defaulted->nodeValue);
$real = $el->attributes->getNamedItemNS('', 'real');
var_dump($real === null ? null : $real->nodeValue);
?>
--EXPECT--
NULL
string(7) "present"
