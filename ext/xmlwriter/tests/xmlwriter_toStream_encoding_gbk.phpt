--TEST--
XMLWriter::toStream() with encoding - test GBK
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$h = fopen("php://output", "w");

$writer = XMLWriter::toStream($h);
$writer->startDocument(encoding: "GBK");
$writer->writeComment("\u{00E9}\u{00E9}\u{00E9}");
unset($writer);

?>
--EXPECT--
<?xml version="1.0" encoding="GBK"?>
<!--ééé-->
