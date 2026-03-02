--TEST--
Bug GH-12170 (Can't use xpath with comments in SimpleXML)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0" encoding="utf-8"?>
<foo>
  <bar>text node</bar>
  <bar><!-- baz --></bar>
  <bar><!-- foo --></bar>
</foo>
XML;

$sxe = simplexml_load_string($xml);

var_dump(
    $sxe->xpath('//bar')
);

foreach ($sxe->xpath('//comment()') as $comment) {
    var_dump($comment->getName());
    var_dump($comment->asXML());
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
    ["comment"]=>
    object(SimpleXMLElement)#5 (0) {
    }
  }
  [2]=>
  object(SimpleXMLElement)#4 (1) {
    ["comment"]=>
    object(SimpleXMLElement)#5 (0) {
    }
  }
}
string(7) "comment"
string(12) "<!-- baz -->"
string(7) "comment"
string(12) "<!-- foo -->"
