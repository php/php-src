--TEST--
GH-12192 (SimpleXML infinite loop when getName() is called within foreach)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = "<root><a>1</a><a>2</a></root>";
$xml = simplexml_load_string($xml);

$a = $xml->a;

foreach ($a as $test) {
    echo "Iteration\n";
    var_dump($a->key());
    var_dump($a->getName());
    var_dump((string) $test);
}

var_dump($a);

?>
--EXPECT--
Iteration
string(1) "a"
string(1) "a"
string(1) "1"
Iteration
string(1) "a"
string(1) "a"
string(1) "2"
object(SimpleXMLElement)#2 (2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
