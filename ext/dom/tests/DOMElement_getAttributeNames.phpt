--TEST--
DOMElement::getAttributeNames()
--EXTENSIONS--
dom
--FILE--
<?php

function test($str) {
    $dom = new DOMDocument();
    $dom->loadXML($str);
    var_dump($dom->documentElement->getAttributeNames());
    foreach ($dom->documentElement->getAttributeNames() as $name) {
        assert($dom->documentElement->getAttributeNode($name)->nodeName === $name);
    }
}

test('<html xmlns:some="some:ns" some:test="a" test2="b"/>');
test('<html test="b" test3="c"/>');
test('<html/>');

?>
--EXPECT--
array(3) {
  [0]=>
  string(10) "xmlns:some"
  [1]=>
  string(9) "some:test"
  [2]=>
  string(5) "test2"
}
array(2) {
  [0]=>
  string(4) "test"
  [1]=>
  string(5) "test3"
}
array(0) {
}
