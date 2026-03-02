--TEST--
XMLWriter::toStream() with encoding - test UTF-8
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$h = fopen("php://output", "w");

$writer = XMLWriter::toStream($h);
$writer->startDocument(encoding: "UTF-8");
$writer->writeComment('ééé');
unset($writer);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<!--ééé-->
