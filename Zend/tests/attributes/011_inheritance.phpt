--TEST--
Attributes comply with inheritance rules.
--FILE--
<?php

#[A2]
class C1
{
    #[A1]
    public function foo() { }
}

class C2 extends C1
{
    public function foo() { }
}

class C3 extends C1
{
    #[A1]
    public function bar() { }
}

$ref = new \ReflectionClass(C1::class);
print_r(array_map(fn ($a) => $a->getName(), $ref->getAttributes()));
print_r(array_map(fn ($a) => $a->getName(), $ref->getMethod('foo')->getAttributes()));

$ref = new \ReflectionClass(C2::class);
print_r(array_map(fn ($a) => $a->getName(), $ref->getAttributes()));
print_r(array_map(fn ($a) => $a->getName(), $ref->getMethod('foo')->getAttributes()));

$ref = new \ReflectionClass(C3::class);
print_r(array_map(fn ($a) => $a->getName(), $ref->getAttributes()));
print_r(array_map(fn ($a) => $a->getName(), $ref->getMethod('foo')->getAttributes()));

echo "\n";

trait T1
{
    #[A2]
    public $a;
}

class C4
{
    use T1;
}

class C5
{
    use T1;

    public $a;
}

$ref = new \ReflectionClass(T1::class);
print_r(array_map(fn ($a) => $a->getName(), $ref->getProperty('a')->getAttributes()));

$ref = new \ReflectionClass(C4::class);
print_r(array_map(fn ($a) => $a->getName(), $ref->getProperty('a')->getAttributes()));

$ref = new \ReflectionClass(C5::class);
print_r(array_map(fn ($a) => $a->getName(), $ref->getProperty('a')->getAttributes()));

?>
--EXPECT--
Array
(
    [0] => A2
)
Array
(
    [0] => A1
)
Array
(
)
Array
(
)
Array
(
)
Array
(
    [0] => A1
)

Array
(
    [0] => A2
)
Array
(
    [0] => A2
)
Array
(
)
