--TEST--
Dom\HTMLDocument::createFromFile() with Dom\HTML_NO_DEFAULT_NS
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromFile(__DIR__ . "/paragraph.html", LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
$xpath = new Dom\XPath($dom);
$xpath->registerNamespace("x", "http://www.w3.org/1999/xhtml");
var_dump($xpath->query("//p"));
var_dump($xpath->query("//x:p"));

$dom = Dom\HTMLDocument::createFromFile(__DIR__ . "/paragraph.html", LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR | Dom\HTML_NO_DEFAULT_NS);
$xpath = new Dom\XPath($dom);
$xpath->registerNamespace("x", "http://www.w3.org/1999/xhtml");
var_dump($xpath->query("//p"));
var_dump($xpath->query("//x:p"));

?>
--EXPECT--
object(Dom\NodeList)#3 (1) {
  ["length"]=>
  int(0)
}
object(Dom\NodeList)#4 (1) {
  ["length"]=>
  int(1)
}
object(Dom\NodeList)#3 (1) {
  ["length"]=>
  int(1)
}
object(Dom\NodeList)#3 (1) {
  ["length"]=>
  int(0)
}
