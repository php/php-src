--TEST--
Bug #50146 (property_exists: Closure object cannot have properties)
--FILE--
<?php

$obj = function(){};

var_dump(property_exists($obj,'foo'));

$ref = new ReflectionObject($obj);
var_dump($ref->hasProperty('b'));

var_dump(isset($obj->a));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
