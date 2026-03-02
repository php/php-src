--TEST--
XMLWriter::toMemory() with combinations of empty flush and non-empty flush
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$writer = XMLWriter::toMemory();
var_dump($writer->flush(empty: false));
$writer->startElement('foo');
var_dump($writer->flush(empty: false));
$writer->endElement();
var_dump($writer->flush(empty: false));
var_dump($writer->flush());
var_dump($writer->flush());

?>
--EXPECT--
string(0) ""
string(4) "<foo"
string(6) "<foo/>"
string(6) "<foo/>"
string(0) ""
