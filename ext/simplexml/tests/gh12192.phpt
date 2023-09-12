--TEST--
GH-12192 (SimpleXML infinite loop when getName() is called within foreach)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = "<root><a>foo</a></root>";
$xml = simplexml_load_string($xml);

$a = $xml->a;

foreach ($a as $test) {
    var_dump($a->key());
    var_dump($a->getName());
}

var_dump($a);

?>
--EXPECT--
string(1) "a"
string(1) "a"
object(SimpleXMLElement)#2 (1) {
  [0]=>
  string(3) "foo"
}
