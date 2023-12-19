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
$container = $dom->appendChild($dom->createElement("container"));
$container->appendChild($dom->importNode($cdata));
echo $dom->saveHTML();

?>
--EXPECT--
<container>foobaré"&lt;&gt;-&amp;</container>
