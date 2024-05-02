--TEST--
DOM\XMLDocument::createEmpty 03
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty("1.0", "ASCII");
$dom->append($dom->createElement("foo"));
echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0" encoding="ASCII"?>
<foo/>
