--TEST--
FR #79344 (xmlwriter_write_attribute_ns: $prefix should be nullable)
--SKIPIF--
<?php
if (!extension_loaded('xmlwriter')) die('skip xmlwriter extension not available');
?>
--FILE--
<?php
$writer = new XMLWriter;
$writer->openMemory();
$writer->setIndent(true);
$writer->startElement('foo');

$writer->writeAttributeNS(null, 'test1', null, 'test1');
$writer->startAttributeNS(null, 'test2', null);
$writer->text('test2');
$writer->endAttribute();

$writer->endElement();
echo $writer->outputMemory();
?>
--EXPECT--
<foo test1="test1" test2="test2"/>
