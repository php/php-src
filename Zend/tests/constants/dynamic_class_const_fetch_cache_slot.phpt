--TEST--
Dynamic class constant fetch
--FILE--
<?php

class FooParent {
    public const BAR = 'bar';
    public const BAZ = 'baz';
}

class Foo extends FooParent {
    public const BAZ = 'baz child';
}

class BarParent {
    public const BAR = 'bar 2';
    public const BAZ = 'baz 2';
}

class Bar extends BarParent {
    public const BAZ = 'baz 2 child';
}

function test($const) {
    echo Foo::{$const}, "\n";
    $foo = 'Foo';
    echo $foo::{$const}, "\n";
}

test('BAR');
test('BAZ');

$c = function ($const) {
    echo self::{$const}, "\n";
    echo static::{$const}, "\n";
    echo parent::{$const}, "\n";
};

$c->bindTo(null, Foo::class)('BAR');
$c->bindTo(null, Bar::class)('BAZ');
$c->bindTo(null, Foo::class)('class');
$c->bindTo(null, Bar::class)('class');

?>
--EXPECT--
bar
bar
baz child
baz child
bar
bar
bar
baz 2 child
baz 2 child
baz 2
Foo
Foo
FooParent
Bar
Bar
BarParent
