--TEST--
Literal types: ReflectionLiteralScalarType::getValue()
--FILE--
<?php
class Foo {}

function f(1|2|'foo'|3.14 $union, 42 $int, -1 $neg, "x" $str, 1|null $nullable,
           int $scalar, Foo $class): void {}

$params = (new ReflectionFunction('f'))->getParameters();

function dump(ReflectionType $t): void {
    if ($t instanceof ReflectionLiteralScalarType) {
        $v = $t->getValue();
        printf("%-7s %s value=%s (%s)\n", (string) $t, get_class($t), var_export($v, true), gettype($v));
    } else {
        printf("%-7s %s\n", (string) $t, get_class($t));
    }
}

foreach ($params[0]->getType()->getTypes() as $m) {
    dump($m);
}
dump($params[1]->getType());
dump($params[2]->getType());
dump($params[3]->getType());
dump($params[4]->getType());
dump($params[5]->getType());
dump($params[6]->getType());
?>
--EXPECT--
1       ReflectionLiteralScalarType value=1 (integer)
2       ReflectionLiteralScalarType value=2 (integer)
'foo'   ReflectionLiteralScalarType value='foo' (string)
3.14    ReflectionLiteralScalarType value=3.14 (double)
42      ReflectionLiteralScalarType value=42 (integer)
-1      ReflectionLiteralScalarType value=-1 (integer)
'x'     ReflectionLiteralScalarType value='x' (string)
?1      ReflectionLiteralScalarType value=1 (integer)
int     ReflectionNamedType
Foo     ReflectionNamedType
