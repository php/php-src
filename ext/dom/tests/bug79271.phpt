--TEST--
Bug #79271 (DOMDocumentType::$childNodes is NULL)
--SKIPIF--
<?php
if (!extension_loaded('dom')) die('skip dom extension not available');
?>
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
