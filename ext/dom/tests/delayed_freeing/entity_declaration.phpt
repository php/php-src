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
--EXPECTF--
string(4) "test"
string(5) "books"

Fatal error: Uncaught Error: Couldn't fetch DOMEntity. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
