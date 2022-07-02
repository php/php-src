--TEST--
Bug #72971 (2): SimpleXML property write does not respect namespace
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = new SimpleXMLElement('<root xmlns:ns="ns"><foo>bar</foo><ns:foo>ns:bar</ns:foo></root>');

$xml->foo = 'new-bar';
var_dump($xml->foo);
var_dump($xml->children('ns')->foo);

$xml->children('ns')->foo = 'ns:new-bar';
var_dump($xml->foo);
var_dump($xml->children('ns')->foo);

?>
--EXPECT--
object(SimpleXMLElement)#2 (1) {
  [0]=>
  string(7) "new-bar"
}
object(SimpleXMLElement)#3 (1) {
  [0]=>
  string(6) "ns:bar"
}
object(SimpleXMLElement)#3 (1) {
  [0]=>
  string(7) "new-bar"
}
object(SimpleXMLElement)#2 (1) {
  [0]=>
  string(10) "ns:new-bar"
}
