--TEST--
GH-18304 (Changing the properties of a DateInterval through dynamic properties triggers a SegFault)
--CREDITS--
orose-assetgo
--EXTENSIONS--
simplexml
--FILE--
<?php
$sxe = simplexml_load_string('<root><abc/></root>');
$field = 'abc';
$sxe->$field .= 'hello';
var_dump($sxe->$field);
?>
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(5) "hello"
}
