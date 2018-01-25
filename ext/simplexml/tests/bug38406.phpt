--TEST--
Bug #38406 (crash when assigning objects to SimpleXML attributes)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$item = new SimpleXMLElement('<something />');
$item->attribute = 'something';
var_dump($item->attribute);

$item->otherAttribute = $item->attribute;
var_dump($item->otherAttribute);

$a = array();
$item->$a = new stdclass;

echo "Done\n";
?>
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(9) "something"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(9) "something"
}

Notice: Array to string conversion in %s on line %d

Warning: It is not yet possible to assign complex types to properties in %s on line %d
Done
