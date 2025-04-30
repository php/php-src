--TEST--
GH-16149 (Null pointer dereference in DOMElement->getAttributeNames())
--EXTENSIONS--
dom
--FILE--
<?php
$element = new DOMElement("b", null, "a");
var_dump($element->getAttributeNames());
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "xmlns"
}
