--TEST--
ReflectionObject::__construct - invalid arguments
--FILE--
<?php

var_dump(new ReflectionObject());
var_dump(new ReflectionObject('stdClass'));
$myInstance = new stdClass;
var_dump(new ReflectionObject($myInstance, $myInstance));
var_dump(new ReflectionObject(0));
var_dump(new ReflectionObject(null));
var_dump(new ReflectionObject(array(1,2)));
?>
--EXPECTF--
Warning: ReflectionObject::__construct() expects exactly 1 parameter, 0 given in %s on line 3
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(0) ""
}

Warning: ReflectionObject::__construct() expects parameter 1 to be object, string given in %s on line 4
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(0) ""
}

Warning: ReflectionObject::__construct() expects exactly 1 parameter, 2 given in %s on line 6
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(0) ""
}

Warning: ReflectionObject::__construct() expects parameter 1 to be object, integer given in %s on line 7
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(0) ""
}

Warning: ReflectionObject::__construct() expects parameter 1 to be object, null given in %s on line 8
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(0) ""
}

Warning: ReflectionObject::__construct() expects parameter 1 to be object, array given in %s on line 9
object(ReflectionObject)#%d (1) {
  ["name"]=>
  string(0) ""
}
