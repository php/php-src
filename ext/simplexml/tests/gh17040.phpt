--TEST--
GH-17040 (SimpleXML's unset can break DOM objects)
--EXTENSIONS--
dom
simplexml
--FILE--
<?php
$dom = new DOMDocument;
$tag = $dom->appendChild($dom->createElement("style"));
$html = simplexml_import_dom($tag);
unset($html[0]);
$tag->append("foo");
echo $dom->saveXML(), "\n";
echo $dom->saveXML($tag), "\n";
var_dump($html);
?>
--EXPECT--
<?xml version="1.0"?>

<style>foo</style>
object(SimpleXMLElement)#3 (1) {
  [0]=>
  string(3) "foo"
}
