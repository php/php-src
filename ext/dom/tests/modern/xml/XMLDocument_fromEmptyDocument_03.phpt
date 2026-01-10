--TEST--
Dom\XMLDocument::createEmpty 03
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty("1.0", "ASCII");
$dom->append($dom->createElement("foo"));
echo $dom->saveXml();

?>
--EXPECT--
<?xml version="1.0" encoding="ASCII"?>
<foo/>
