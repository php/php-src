--TEST--
Bug #72971: SimpleXML isset/unset do not respect namespace
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip simplexml extension is not loaded"; ?>
--FILE--
<?php

$xml = new SimpleXMLElement('<root xmlns:ns="ns"><foo>bar</foo><ns:foo>ns:bar</ns:foo><ns:foo2>ns:bar2</ns:foo2></root>');
var_dump(isset($xml->foo2));
unset($xml->foo);
var_dump($xml->children('ns'));

?>
--EXPECT--
bool(false)
object(SimpleXMLElement)#2 (2) {
  ["foo"]=>
  string(6) "ns:bar"
  ["foo2"]=>
  string(7) "ns:bar2"
}
