--TEST--
Literal Variables in Function Declarations
--FILE--
<?php
#[\Bar("parameter")]
function foo($qux) {}

$reflector = new ReflectionFunction('foo');

var_dump(
    $reflector->getName(), 
        is_literal($reflector->getName()));

foreach ($reflector->getAttributes() as $attribute) {
    $parameters = $attribute->getArguments();
    
    var_dump(
        $attribute->getName(),
            is_literal($attribute->getName()));

    foreach ($parameters as $parameter => $value) {
        var_dump(
            $value,
            is_literal($value));
    }
}

foreach ($reflector->getParameters() as $parameter) {
    var_dump(
        $parameter->getName(),
        is_literal($parameter->getName()));
}
?>
--EXPECT--
string(3) "foo"
bool(true)
string(3) "Bar"
bool(true)
string(9) "parameter"
bool(true)
string(3) "qux"
bool(true)
