--TEST--
DOM\HTML5Document serialization of processing instruction
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
// Note: you can put > in a processing instruction element but that breaks (as expected)
$dom->appendChild($dom->createProcessingInstruction("target", "foobaré\"&<\xc2\xa0"));
echo $dom->saveHTML();

?>
--EXPECT--
<?target foobaré"&< >
