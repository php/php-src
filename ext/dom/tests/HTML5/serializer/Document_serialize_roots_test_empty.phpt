--TEST--
DOM\HTML5Document serialization of different roots resulting in an empty result
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$comment = $dom->appendChild($dom->createComment("comment"));
$cdata = $dom->appendChild($dom->createCDATASection("cdata"));
$emptyElement = $dom->appendChild($dom->createElement("empty"));
$text = $dom->appendChild($dom->createTextNode("text"));
$pi = $dom->appendChild($dom->createProcessingInstruction("target", "data"));
$fragment = $dom->createDocumentFragment();

var_dump($dom->saveHTML($comment));
var_dump($dom->saveHTML($cdata));
var_dump($dom->saveHTML($emptyElement));
var_dump($dom->saveHTML($text));
var_dump($dom->saveHTML($pi));
var_dump($dom->saveHTML($fragment));

?>
--EXPECT--
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
