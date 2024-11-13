--TEST--
Dom\HTMLDocument serialization of CData
--EXTENSIONS--
dom
--FILE--
<?php

// Note: can't create CData in an HTML document.
$dom = Dom\XMLDocument::createEmpty();
$cdata = $dom->createCDATASection("foobaré\"<>-&");

$dom = Dom\HTMLDocument::createEmpty();
$container = $dom->appendChild($dom->createElement("container"));
$container->appendChild($dom->importNode($cdata));
echo $dom->saveHtml();

?>
--EXPECT--
<container>foobaré"&lt;&gt;-&amp;</container>
