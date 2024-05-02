--TEST--
DOM\HTMLDocument::createFromString() with DOM\HTML_NO_DEFAULT_NS
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString(file_get_contents(__DIR__ . "/paragraph.html"), LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
$xpath = new DOM\XPath($dom);
$xpath->registerNamespace("x", "http://www.w3.org/1999/xhtml");
var_dump($xpath->query("//p"));
var_dump($xpath->query("//x:p"));

$dom = DOM\HTMLDocument::createFromString(file_get_contents(__DIR__ . "/paragraph.html"), LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR | DOM\HTML_NO_DEFAULT_NS);
$xpath = new DOM\XPath($dom);
$xpath->registerNamespace("x", "http://www.w3.org/1999/xhtml");
var_dump($xpath->query("//p"));
var_dump($xpath->query("//x:p"));

?>
--EXPECT--
object(DOM\NodeList)#3 (1) {
  ["length"]=>
  int(0)
}
object(DOM\NodeList)#4 (1) {
  ["length"]=>
  int(1)
}
object(DOM\NodeList)#3 (1) {
  ["length"]=>
  int(1)
}
object(DOM\NodeList)#3 (1) {
  ["length"]=>
  int(0)
}
