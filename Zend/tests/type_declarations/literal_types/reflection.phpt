--TEST--
Literal types: reflection (union members, single literal, nullable)
--FILE--
<?php
function f(1|2|'foo' $a, 42 $b, 1|null $c): void {}

$params = (new ReflectionFunction('f'))->getParameters();

$union = $params[0]->getType();
printf("a: %s | %s\n", get_class($union), (string) $union);
foreach ($union->getTypes() as $m) {
    printf("   %s value=%s\n", get_class($m), var_export($m->getValue(), true));
}

$single = $params[1]->getType();
printf("b: %s | %s | value=%s | nullable=%s\n",
    get_class($single), (string) $single, var_export($single->getValue(), true),
    var_export($single->allowsNull(), true));

$nullable = $params[2]->getType();
printf("c: %s | %s | value=%s | nullable=%s\n",
    get_class($nullable), (string) $nullable, var_export($nullable->getValue(), true),
    var_export($nullable->allowsNull(), true));
?>
--EXPECT--
a: ReflectionUnionType | 1|2|'foo'
   ReflectionLiteralScalarType value=1
   ReflectionLiteralScalarType value=2
   ReflectionLiteralScalarType value='foo'
b: ReflectionLiteralScalarType | 42 | value=42 | nullable=false
c: ReflectionLiteralScalarType | ?1 | value=1 | nullable=true
