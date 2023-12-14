--TEST--
DOM\HTMLDocument serialization of CData
--EXTENSIONS--
dom
--FILE--
<?php

// Note: can't create CData in an HTML document.
$dom = DOM\XMLDocument::createEmpty();
$cdata = $dom->createCDATASection("foobaré\"<>-&");

$dom = DOM\HTMLDocument::createEmpty();
$dom->appendChild($dom->importNode($cdata));
echo $dom->saveHTML();

?>
--EXPECT--
foobaré"&lt;&gt;-&amp;
