--TEST--
ReflectionParameter class - isPassedByReferenceMethod()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class ReflectTestClass {
    public static function staticMethod(&$paramOne, $anotherParam) {
        return ++$theIncrement;
    }
    
    public function instanceMethod($firstParam, &$secondParam) {
      $firstParam = "Hello\n";
    }
}

// Create an instance of the Reflection_Method class
$method = new ReflectionMethod('ReflectTestClass', 'staticMethod');
// Get the parameters
$parameters = $method->getParameters();
echo "Parameters from staticMethod:\n\n";
foreach($parameters as $parameter) {
	var_dump($parameter);
    if($parameter->isPassedByReference()) {
    	echo "This param is passed by reference\n";
    } else {
    	echo "This param is not passed by reference\n";
    }
    echo "\n";
}

// Create an instance of the Reflection_Method class
$method = new ReflectionMethod('ReflectTestClass', 'instanceMethod');
// Get the parameters
$parameters = $method->getParameters();
echo "Parameters from instanceMethod:\n\n";
foreach($parameters as $parameter) {
	var_dump($parameter);
    if($parameter->isPassedByReference()) {
    	echo "This param is passed by reference\n";
    } else {
    	echo "This param is not passed by reference\n";
    }
    echo "\n";
}

echo "done\n";

?>
--EXPECTF--
Parameters from staticMethod:

object(ReflectionParameter)#%i (1) {
  ["name"]=>
  string(8) "paramOne"
}
This param is passed by reference

object(ReflectionParameter)#%i (1) {
  ["name"]=>
  string(12) "anotherParam"
}
This param is not passed by reference

Parameters from instanceMethod:

object(ReflectionParameter)#%i (1) {
  ["name"]=>
  string(10) "firstParam"
}
This param is not passed by reference

object(ReflectionParameter)#%i (1) {
  ["name"]=>
  string(11) "secondParam"
}
This param is passed by reference

done