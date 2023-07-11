--TEST--
Delayed freeing entity declaration
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<'XML'
<?xml version="1.0"?>
<!DOCTYPE books [
<!ENTITY test "entity is only for test purposes">
]>
<container/>
XML);
$entity = $doc->doctype->entities[0];
var_dump($entity->nodeName, $entity->parentNode->nodeName);
$doc->removeChild($doc->doctype);
var_dump($entity->nodeName, $entity->parentNode);
?>
--EXPECT--
string(4) "test"
string(5) "books"
string(4) "test"
NULL
