--TEST--
Bug #38406 (crash when assigning objects to SimpleXML attributes)
--EXTENSIONS--
simplexml
--FILE--
<?php

$item = new SimpleXMLElement('<something />');
$item->attribute = 'something';
var_dump($item->attribute);

$item->otherAttribute = $item->attribute;
var_dump($item->otherAttribute);

$a = array();

try {
    $item->$a = new stdclass;
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

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

Warning: Array to string conversion in %s on line %d
It's not possible to assign a complex type to properties, stdClass given
Done
