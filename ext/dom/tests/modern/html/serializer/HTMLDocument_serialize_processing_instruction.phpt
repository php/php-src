--TEST--
Dom\HTMLDocument serialization of processing instruction
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
// Note: you can put > in a processing instruction element but that breaks (as expected)
$dom->appendChild($dom->createProcessingInstruction("target", "foobaré\"&<\xc2\xa0"));
echo $dom->saveHtml();

?>
--EXPECT--
<?target foobaré"&< >
