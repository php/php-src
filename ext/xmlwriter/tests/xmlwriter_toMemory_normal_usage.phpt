--TEST--
XMLWriter::toMemory() - normal usage
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$writer = XMLWriter::toMemory();
$writer->startElement("root");
$writer->writeAttribute("align", "left");
$writer->writeComment("hello");
$writer->endElement();
echo $writer->outputMemory();

?>
--EXPECT--
<root align="left"><!--hello--></root>
