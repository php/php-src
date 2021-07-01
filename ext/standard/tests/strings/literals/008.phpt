--TEST--
Literal Variables in Class Declarations
--FILE--
<?php
#[\Bar("parameter")]
class Foo {
    const LITERAL = "LITERAL";
    static $PROPERTY = "STRING"; 
    
    public function method($qux) {}
}

$reflector = new ReflectionClass(Foo::class);

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

$reflector = new ReflectionMethod(Foo::class, "method");

foreach ($reflector->getParameters() as $parameter) {
    var_dump(
        $parameter->getName(),
        is_literal($parameter->getName()));
}

var_dump(
    FOO::LITERAL,
        is_literal(FOO::LITERAL),
    Foo::$PROPERTY,
        is_literal(FOO::$PROPERTY));
?>
--EXPECT--
string(3) "Foo"
bool(true)
string(3) "Bar"
bool(true)
string(9) "parameter"
bool(true)
string(3) "qux"
bool(true)
string(7) "LITERAL"
bool(true)
string(6) "STRING"
bool(true)
