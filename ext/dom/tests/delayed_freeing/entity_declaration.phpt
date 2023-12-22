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
<!ENTITY myimage PUBLIC "-" "mypicture.gif" NDATA GIF>
]>
<container/>
XML);
$ref1 = $doc->createEntityReference("test");
$ref2 = $doc->createEntityReference("myimage");
$entity1 = $doc->doctype->entities[0];
var_dump($entity1->nodeName, $entity1->parentNode->nodeName);
$entity2 = $doc->doctype->entities[1];
var_dump($entity2->nodeName, $entity2->parentNode->nodeName);
$doc->removeChild($doc->doctype);
var_dump($entity1->nodeName, $entity1->parentNode);
var_dump($entity2->nodeName, $entity2->parentNode);
?>
--EXPECT--
string(4) "test"
string(5) "books"
string(7) "myimage"
string(5) "books"
string(4) "test"
NULL
string(7) "myimage"
NULL
