--TEST--
ReflectionParameter class - getNames() method.
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class ReflectTestClass {
    public static function twoArgFunction($theIncrement, $anotherParam) {
        return ++$theIncrement;
    }
    
    public function oneArgNonStatic($theParam) {
    	$theParam--;
    }
    
    public function noArgs() {
    	echo "No arg function\n";
    }
}

// Create an instance of the Reflection_Method class
$method = new ReflectionMethod('ReflectTestClass', 'twoArgFunction');
// Get the parameters
$parameters = $method->getParameters();
echo "Parameters from twoArgMethod:\n\n";
foreach($parameters as $parameter) {
	var_dump($parameter);
	$name = $parameter->getName();
	echo "\n";
}

$method = new ReflectionMethod('ReflectTestClass', 'oneArgNonStatic');
$parameters = $method->getParameters();
echo "Parameters from oneArgNonStatic:\n\n";
foreach($parameters as $parameter) {
	var_dump($parameter);
	$name = $parameter->getName();
	echo "\n";
}


$method = new ReflectionMethod('ReflectTestClass', 'noArgs');
$parameters = $method->getParameters();
echo "Parameters from noArgs:\n\n";
var_dump($parameters);
foreach($parameters as $parameter) {
	var_dump($parameter);
	$name = $parameter->getName();
	echo "\n";
}

echo "done\n";

?>
--EXPECTF--
Parameters from twoArgMethod:

object(ReflectionParameter)#%i (1) {
  ["name"]=>
  string(12) "theIncrement"
}

object(ReflectionParameter)#%i (1) {
  ["name"]=>
  string(12) "anotherParam"
}

Parameters from oneArgNonStatic:

object(ReflectionParameter)#%i (1) {
  ["name"]=>
  string(8) "theParam"
}

Parameters from noArgs:

array(0) {
}
done