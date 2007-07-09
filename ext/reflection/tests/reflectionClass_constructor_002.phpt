--TEST--
ReflectionClass::__constructor() - bad arguments
--FILE--
<?php
try {
	var_dump(new ReflectionClass());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";  
}

try {
	var_dump(new ReflectionClass(null));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";  
}

try {
	var_dump(new ReflectionClass(true));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";  
}

try {
	var_dump(new ReflectionClass(1));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";  
}

try {
	var_dump(new ReflectionClass(array(1,2,3)));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";  
}

try {
	var_dump(new ReflectionClass("stdClass", 1));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";  
}

try {
	var_dump(new ReflectionClass("X"));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";  
}

?>
--EXPECTF--

Warning: ReflectionClass::__construct() expects exactly 1 parameter, 0 given in %s on line 3
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(0) ""
}
Class  does not exist
Class 1 does not exist
Class 1 does not exist

Warning: ReflectionClass::__construct() expects parameter 1 to be string (Unicode or binary), array given in %s on line 27
object(ReflectionClass)#3 (1) {
  ["name"]=>
  string(0) ""
}

Warning: ReflectionClass::__construct() expects exactly 1 parameter, 2 given in %s on line 33
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(0) ""
}
Class X does not exist
--UEXPECTF--

Warning: ReflectionClass::__construct() expects exactly 1 parameter, 0 given in %s on line 3
object(ReflectionClass)#%d (1) {
  [u"name"]=>
  unicode(0) ""
}
Class  does not exist
Class 1 does not exist
Class 1 does not exist

Warning: ReflectionClass::__construct() expects parameter 1 to be string (Unicode or binary), array given in %s on line 27
object(ReflectionClass)#3 (1) {
  [u"name"]=>
  unicode(0) ""
}

Warning: ReflectionClass::__construct() expects exactly 1 parameter, 2 given in %s on line 33
object(ReflectionClass)#%d (1) {
  [u"name"]=>
  unicode(0) ""
}
Class X does not exist
