--TEST--
Bug #40794 (ReflectionObject::getValues() may crash when used with dynamic properties)
--FILE--
<?php

$obj = new stdClass();
$obj->prop1 = '1';
$obj->prop2 = '2';
$obj->prop3 = '3';

$reflect = new ReflectionObject($obj);

$array = array();
foreach($reflect->getProperties() as $prop)
{
	$array[$prop->getName()] = $prop->getValue($obj);
}

var_dump($array);

echo "Done\n";
?>
--EXPECTF--
array(3) {
  ["prop1"]=>
  string(1) "1"
  ["prop2"]=>
  string(1) "2"
  ["prop3"]=>
  string(1) "3"
}
Done
