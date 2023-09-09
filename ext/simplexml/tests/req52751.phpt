--TEST--
Request #52751 (XPath processing-instruction() function is not supported)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0" encoding="utf-8"?>
<foo>
  <bar>text node</bar>
  <bar><?baz href="ftw" ?></bar>
  <bar><?foo bar ?></bar>
</foo>
XML;

$sxe = simplexml_load_string($xml);

var_dump(
    $sxe->xpath('//bar')
);

var_dump(
    $sxe->xpath('//processing-instruction(\'baz\')')
);

foreach ($sxe->xpath('//processing-instruction()') as $pi) {
    var_dump($pi->getName());
}

?>
--EXPECT--
array(3) {
  [0]=>
  object(SimpleXMLElement)#2 (1) {
    [0]=>
    string(9) "text node"
  }
  [1]=>
  object(SimpleXMLElement)#3 (1) {
    ["baz"]=>
    object(SimpleXMLElement)#5 (0) {
    }
  }
  [2]=>
  object(SimpleXMLElement)#4 (1) {
    ["foo"]=>
    object(SimpleXMLElement)#5 (0) {
    }
  }
}
array(1) {
  [0]=>
  object(SimpleXMLElement)#4 (0) {
  }
}
string(3) "baz"
string(3) "foo"
