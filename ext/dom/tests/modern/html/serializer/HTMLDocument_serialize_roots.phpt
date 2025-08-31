--TEST--
Dom\HTMLDocument serialization of different roots
--EXTENSIONS--
dom
--FILE--
<?php

// Note: can't create CData in an HTML document.
$dom = Dom\XMLDocument::createEmpty();
$cdata = $dom->createCDATASection("cdata");

$dom = Dom\HTMLDocument::createEmpty();
$container = $dom->appendChild($dom->createElement("container"));
$comment = $container->appendChild($dom->createComment("comment"));
$cdata = $container->appendChild($dom->importNode($cdata));
$emptyElement = $container->appendChild($dom->createElement("empty"));
$text = $container->appendChild($dom->createTextNode("text"));
$pi = $container->appendChild($dom->createProcessingInstruction("target", "data"));
$fragment = $dom->createDocumentFragment();

var_dump($dom->saveHtml($comment));
var_dump($dom->saveHtml($cdata));
var_dump($dom->saveHtml($emptyElement));
var_dump($dom->saveHtml($text));
var_dump($dom->saveHtml($pi));
var_dump($dom->saveHtml($fragment));

?>
--EXPECT--
string(14) "<!--comment-->"
string(5) "cdata"
string(15) "<empty></empty>"
string(4) "text"
string(14) "<?target data>"
string(0) ""
