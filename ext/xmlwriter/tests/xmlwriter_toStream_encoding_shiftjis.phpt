--TEST--
XMLWriter::toStream() with encoding - test SHIFT_JIS
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$h = fopen("php://output", "w");

$writer = XMLWriter::toStream($h);
$writer->startDocument(encoding: "SHIFT_JIS");
$writer->writeComment("\u{3041}\u{3041}\u{3041}");
unset($writer);

?>
--EXPECT--
<?xml version="1.0" encoding="SHIFT_JIS"?>
<!--ぁぁぁ-->
