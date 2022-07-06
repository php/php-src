--TEST--
Bug #50661 (DOMDocument::loadXML does not allow UTF-16).
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$data = "\xFE\xFF\x00\x3C\x00\x66\x00\x6F\x00\x6F\x00\x2F\x00\x3E";

$dom = new DOMDocument();
$dom->loadXML($data);
echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<foo/>
