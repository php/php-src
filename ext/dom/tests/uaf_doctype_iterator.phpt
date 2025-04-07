--TEST--
UAF when removing doctype and iterating over the child nodes
--EXTENSIONS--
dom
--CREDITS--
Yuancheng Jiang
--FILE--
<?php
$dom = new DOMDocument;
$dom->loadXML(<<<XML
<!DOCTYPE foo [
    <!ENTITY foo1 "bar1">
]>
<foo>&foo1;</foo>
XML);
$ref = $dom->documentElement->firstChild;
$nodes = $ref->childNodes;
$dom->removeChild($dom->doctype);
foreach($nodes as $str) {}
var_dump($nodes);
?>
--EXPECTF--
object(DOMNodeList)#%d (1) {
  ["length"]=>
  int(0)
}
