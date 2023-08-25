--TEST--
Document::loadHTML() with DOM\HTML_NO_DEFAULT_NS
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTML(file_get_contents(__DIR__ . "/paragraph.html"), LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
$xpath = new DOMXPath($dom);
$xpath->registerNamespace("x", "http://www.w3.org/1999/xhtml");
var_dump($xpath->query("//p"));
var_dump($xpath->query("//x:p"));

$dom = new DOM\HTML5Document();
$dom->loadHTML(file_get_contents(__DIR__ . "/paragraph.html"), LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR | DOM\HTML_NO_DEFAULT_NS);
$xpath = new DOMXPath($dom);
$xpath->registerNamespace("x", "http://www.w3.org/1999/xhtml");
var_dump($xpath->query("//p"));
var_dump($xpath->query("//x:p"));

?>
--EXPECT--
object(DOMNodeList)#3 (1) {
  ["length"]=>
  int(0)
}
object(DOMNodeList)#4 (1) {
  ["length"]=>
  int(1)
}
object(DOMNodeList)#3 (1) {
  ["length"]=>
  int(1)
}
object(DOMNodeList)#3 (1) {
  ["length"]=>
  int(0)
}
