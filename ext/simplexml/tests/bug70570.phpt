--TEST--
Bug #70570 (json_encode and var_dump ignore simplexml cdata)
--EXTENSIONS--
simplexml
--FILE--
<?php

$input = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<container>
    <A><![CDATA[0]]></A>
    <B><![CDATA[hello world]]></B>
    <C><![CDATA[hello]]><foo/><![CDATA[world]]></C>
    <D><foo/><![CDATA[hello]]><bar/></D>
</container>
XML;

$xml = simplexml_load_string($input);

var_dump($xml);

foreach (["A", "B"] as $item) {
    echo "--- Testing $item ---\n";
    var_dump($xml->xpath("//$item/text()"));
    var_dump($xml->$item);
    var_dump(isset($xml->$item));
    var_dump(empty($xml->$item));
    var_dump((bool) ($xml->$item));
}

echo "--- Testing C ---\n";

var_dump($xml->C); // Equivalent to how (normal) text nodes behave
var_dump((string) $xml->C);
var_dump($xml->C->foo);

?>
--EXPECT--
object(SimpleXMLElement)#1 (4) {
  ["A"]=>
  string(1) "0"
  ["B"]=>
  string(11) "hello world"
  ["C"]=>
  string(10) "helloworld"
  ["D"]=>
  object(SimpleXMLElement)#2 (2) {
    ["foo"]=>
    object(SimpleXMLElement)#3 (0) {
    }
    ["bar"]=>
    object(SimpleXMLElement)#4 (0) {
    }
  }
}
--- Testing A ---
array(1) {
  [0]=>
  object(SimpleXMLElement)#2 (1) {
    [0]=>
    string(1) "0"
  }
}
object(SimpleXMLElement)#2 (1) {
  [0]=>
  string(1) "0"
}
bool(true)
bool(true)
bool(true)
--- Testing B ---
array(1) {
  [0]=>
  object(SimpleXMLElement)#2 (1) {
    [0]=>
    string(11) "hello world"
  }
}
object(SimpleXMLElement)#2 (1) {
  [0]=>
  string(11) "hello world"
}
bool(true)
bool(false)
bool(true)
--- Testing C ---
object(SimpleXMLElement)#2 (1) {
  ["foo"]=>
  object(SimpleXMLElement)#3 (0) {
  }
}
string(10) "helloworld"
object(SimpleXMLElement)#3 (0) {
}
