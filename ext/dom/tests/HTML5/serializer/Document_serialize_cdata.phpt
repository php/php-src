--TEST--
DOM\HTML5Document serialization of CData
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->appendChild($dom->createCDATASection("foobaré\"<>-&"));
echo $dom->saveHTML();

?>
--EXPECT--
foobaré"&lt;&gt;-&amp;
