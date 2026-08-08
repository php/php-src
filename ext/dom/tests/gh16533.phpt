--TEST--
GH-16533 (Segfault when adding attribute to parent that is not an element)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
try {
    $doc->appendChild($doc->createAttribute('foo'));
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo $doc->saveXML();

?>
--EXPECT--
DOMException: Hierarchy Request Error
<?xml version="1.0"?>
