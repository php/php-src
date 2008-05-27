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
  [u"prop1"]=>
  unicode(1) "1"
  [u"prop2"]=>
  unicode(1) "2"
  [u"prop3"]=>
  unicode(1) "3"
}
Done
