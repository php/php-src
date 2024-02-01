--TEST--
ReflectionEnumBackedCase::getBackingValue() exception
--FILE--
<?php
enum Inm:int {
    case Foo=y;
}
try {
    var_dump((new ReflectionEnumBackedCase(Inm::class,'Foo'))->getBackingValue());
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Undefined constant "y"
