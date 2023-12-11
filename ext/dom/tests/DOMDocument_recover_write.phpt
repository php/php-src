--TEST--
DOMDocument::recover write
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument;
var_dump($dom->recover);
$dom->recover = true;
var_dump($dom->recover);
echo $dom->saveXML();
?>
--EXPECT--
bool(false)
bool(true)
<?xml version="1.0"?>
