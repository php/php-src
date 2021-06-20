--TEST--
Bug #79271 (DOMDocumentType::$childNodes is NULL)
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMImplementation();
$type = $dom->createDocumentType('html');
var_dump($type->childNodes);
?>
--EXPECTF--
object(DOMNodeList)#%d (1) {
  ["length"]=>
  int(0)
}
