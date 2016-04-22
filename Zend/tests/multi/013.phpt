--TEST--
basic reflection
--FILE--
<?php
$function = function(IFoo | IBar $thing) : IFoo & IBar {

};

$reflector = new ReflectionFunction($function);
$param = 
	$reflector->getParameters()[0]->getType();
$return =
	$reflector->getReturnType();

var_dump((string)$param,
		 (string)$return);

var_dump($param->isUnion(),
		 $return->isIntersection());
?>
--EXPECT--
string(12) "IFoo or IBar"
string(13) "IFoo and IBar"
bool(true)
bool(true)
