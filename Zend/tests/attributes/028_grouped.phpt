--TEST--
Attributes can be grouped
--FILE--
<?php

#[A1(1), A1(2), A2(3)]
class Foo
{
}

#[
    A1(1),
    A1(2),
    A2(3)
]
function foo() {}

#[A1, A1, A2]
function bar() {}

$sources = [
    new \ReflectionClass(Foo::class),
    new \ReflectionFunction('foo'),
    new \ReflectionFunction('bar'),
];

foreach ($sources as $ref) {
    $attr = $ref->getAttributes();
    var_dump(get_class($ref), count($attr));

    foreach ($attr as $a) {
        printf("%s(%s)\n", $a->getName(), implode(", ", $a->getArguments()));
    }

    echo "\n";
}
?>
--EXPECT--
string(15) "ReflectionClass"
int(3)
A1(1)
A1(2)
A2(3)

string(18) "ReflectionFunction"
int(3)
A1(1)
A1(2)
A2(3)

string(18) "ReflectionFunction"
int(3)
A1()
A1()
A2()
