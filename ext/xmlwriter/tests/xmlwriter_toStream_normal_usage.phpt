--TEST--
XMLWriter::toStream() - normal usage
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$h = fopen("php://output", "w");

$writer = XMLWriter::toStream($h);
$writer->startElement("root");
$writer->writeAttribute("align", "left");
$writer->writeComment("hello");
$writer->endElement();
$amount = $writer->flush();
echo "\nFlush amount: ";
var_dump($amount);

// Force destroying the held stream
unset($writer);

// Test that the stream wasn't closed or destroyed
fwrite($h, "\nthis is the end\n");

?>
--EXPECT--
<root align="left"><!--hello--></root>
Flush amount: int(38)

this is the end
