--TEST--
Entity references with stale entity declaration 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML(<<<XML
<!DOCTYPE foo [
<!ENTITY foo "bar">
]>
<foo>&foo;</foo>
XML);

$ref = $dom->documentElement->firstChild;
$decl = $ref->firstChild;

$nodes = $ref->childNodes;
$dom->removeChild($dom->doctype);
unset($decl);

var_dump($nodes);
var_dump($ref->firstChild);
var_dump($ref->lastChild);
var_dump($ref->textContent);
var_dump($ref->childNodes);

?>
--EXPECT--
object(DOMNodeList)#4 (1) {
  ["length"]=>
  int(0)
}
NULL
NULL
string(0) ""
object(DOMNodeList)#2 (1) {
  ["length"]=>
  int(0)
}
