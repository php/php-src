--TEST--
Attributes can be filtered by name and base type.
--FILE--
<?php

$ref = new \ReflectionFunction(#[A1] #[A2] function () { });
$attr = $ref->getAttributes(A3::class);

var_dump(count($attr));

$ref = new \ReflectionFunction(#[A1] #[A2] function () { });
$attr = $ref->getAttributes(A2::class);

var_dump(count($attr), $attr[0]->getName());

$ref = new \ReflectionFunction(#[A1] #[A2] #[A2] function () { });
$attr = $ref->getAttributes(A2::class);

var_dump(count($attr), $attr[0]->getName(), $attr[1]->getName());

echo "\n";

interface Base { }
class A1 implements Base { }
class A2 implements Base { }
class A3 extends A2 { }

$ref = new \ReflectionFunction(#[A1] #[A2] #[A5] function () { });
$attr = $ref->getAttributes(\stdClass::class, \ReflectionAttribute::IS_INSTANCEOF);
var_dump(count($attr));
print_r(array_map(fn ($a) => $a->getName(), $attr));

$ref = new \ReflectionFunction(#[A1] #[A2] function () { });
$attr = $ref->getAttributes(A1::class, \ReflectionAttribute::IS_INSTANCEOF);
var_dump(count($attr));
print_r(array_map(fn ($a) => $a->getName(), $attr));

$ref = new \ReflectionFunction(#[A1] #[A2] function () { });
$attr = $ref->getAttributes(Base::class, \ReflectionAttribute::IS_INSTANCEOF);
var_dump(count($attr));
print_r(array_map(fn ($a) => $a->getName(), $attr));

$ref = new \ReflectionFunction(#[A1] #[A2] #[A3] function () { });
$attr = $ref->getAttributes(A2::class, \ReflectionAttribute::IS_INSTANCEOF);
var_dump(count($attr));
print_r(array_map(fn ($a) => $a->getName(), $attr));

$ref = new \ReflectionFunction(#[A1] #[A2] #[A3] function () { });
$attr = $ref->getAttributes(Base::class, \ReflectionAttribute::IS_INSTANCEOF);
var_dump(count($attr));
print_r(array_map(fn ($a) => $a->getName(), $attr));

echo "\n";

$ref = new \ReflectionFunction(function () { });

try {
    $ref->getAttributes(A1::class, 3);
} catch (\Error $e) {
    echo 'ERROR 1: ', $e::class, ': ', $e->getMessage(), "\n";
}

$ref = new \ReflectionFunction(function () { });

try {
    $ref->getAttributes(SomeMissingClass::class, \ReflectionAttribute::IS_INSTANCEOF);
} catch (\Error $e) {
    echo 'ERROR 2: ', $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
int(0)
int(1)
string(2) "A2"
int(2)
string(2) "A2"
string(2) "A2"

int(0)
Array
(
)
int(1)
Array
(
    [0] => A1
)
int(2)
Array
(
    [0] => A1
    [1] => A2
)
int(2)
Array
(
    [0] => A2
    [1] => A3
)
int(3)
Array
(
    [0] => A1
    [1] => A2
    [2] => A3
)

ERROR 1: ValueError: ReflectionFunctionAbstract::getAttributes(): Argument #2 ($flags) must be a valid attribute filter flag
ERROR 2: Error: Class "SomeMissingClass" not found
