--TEST--
Bug #62639 (XML structure broken)
--EXTENSIONS--
simplexml
--FILE--
<?php

class A extends SimpleXMLElement
{
}

$xml1 = <<<XML
<?xml version="1.0"?>
<a>
    <b>
        <c>
            <value attr="Some Attr">Some Value</value>
        </c>
    </b>
</a>
XML;

$a1 = new A($xml1);

foreach ($a1->b->c->children() as $key => $value) {
    var_dump($value);
}

$xml2 = <<<XML
<?xml version="1.0"?>
<a>
    <b>
        <c><value attr="Some Attr">Some Value</value></c>
    </b>
</a>
XML;

$a2 = new A($xml2);

foreach ($a2->b->c->children() as $key => $value) {
    var_dump($value);
}?>
--EXPECT--
object(A)#2 (2) {
  ["@attributes"]=>
  array(1) {
    ["attr"]=>
    string(9) "Some Attr"
  }
  [0]=>
  string(10) "Some Value"
}
object(A)#3 (2) {
  ["@attributes"]=>
  array(1) {
    ["attr"]=>
    string(9) "Some Attr"
  }
  [0]=>
  string(10) "Some Value"
}
