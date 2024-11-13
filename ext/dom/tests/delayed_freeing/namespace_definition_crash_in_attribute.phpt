--TEST--
Delayed freeing namespace definition should not crash in attribute
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->appendChild($doc->createElement('container'));
$child = $doc->documentElement->appendChild($doc->createElementNS('some:ns', 'child'));
$child_attr1 = $child->appendChild($doc->createAttributeNS('some:ns2', 'hello'));
$child_attr1->textContent = 'hello content 1';
$child_attr2 = $child->appendChild($doc->createAttribute('hello2'));
$child_attr1->textContent = 'hello content 2';
$attr3 = $child->appendChild($doc->createElementNS('some:ns', 'childcontainer'))
        ->appendChild($doc->createAttributeNS('some:ns', 'hello'));

echo $doc->saveXML();

$child->remove();
echo $doc->saveXML();

unset($child);

var_dump($child_attr1->textContent);
var_dump($child_attr2->textContent);
var_dump($child_attr1->namespaceURI);
var_dump($child_attr2->namespaceURI);
var_dump($attr3->textContent);
var_dump($attr3->namespaceURI);

$doc->documentElement->remove();

unset($child_attr1);
unset($child_attr2);
var_dump($attr3->namespaceURI);

echo $doc->saveXML($attr3), "\n";
echo $doc->saveXML($attr3->parentNode), "\n";
?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns:default="some:ns2" xmlns:default1="some:ns"><child xmlns="some:ns" default:hello="hello content 2" hello2=""><childcontainer default1:hello=""/></child></container>
<?xml version="1.0"?>
<container xmlns:default="some:ns2" xmlns:default1="some:ns"/>
string(15) "hello content 2"
string(0) ""
string(8) "some:ns2"
NULL
string(0) ""
string(7) "some:ns"
string(7) "some:ns"
 default1:hello=""
<?xml version="1.0"?>
