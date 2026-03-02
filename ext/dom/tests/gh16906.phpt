--TEST--
GH-16906 (Reloading document can cause UAF in iterator)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML('<?xml version="1.0"?><span><strong id="1"/><strong id="2"/></span>');
$list = $doc->getElementsByTagName('strong');
$doc->load(__DIR__."/book.xml");
var_dump($list);
?>
--EXPECT--
object(DOMNodeList)#2 (1) {
  ["length"]=>
  int(0)
}
