--TEST--
DOM\HTMLDocument serialization of CData
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$dom->appendChild($dom->createCDATASection("foobaré\"<>-&"));
echo $dom->saveHTML();

?>
--EXPECT--
foobaré"&lt;&gt;-&amp;
