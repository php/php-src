--TEST--
ReflectionFunctionAbstract::getClosureUsedVariables
--FILE--
<?php
$reflector = new ReflectionFunction("strlen");

var_dump($reflector->getClosureUsedVariables());

$function = function() {
    static $one = 1;
};

$reflector = new ReflectionFunction($function);

var_dump($reflector->getClosureUsedVariables());

$one = 1;

$function = function() use ($one) {
    static $two = 2;
};

$reflector = new ReflectionFunction($function);

var_dump($reflector->getClosureUsedVariables());

$function = fn() => $two = [$one];

$reflector = new ReflectionFunction($function);

var_dump($reflector->getClosureUsedVariables());

$function = function() use (&$one) {
    static $two = 2;
};

$reflector = new ReflectionFunction($function);

var_dump($reflector->getClosureUsedVariables());
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(1) {
  ["one"]=>
  int(1)
}
array(1) {
  ["one"]=>
  int(1)
}
array(1) {
  ["one"]=>
  &int(1)
}
