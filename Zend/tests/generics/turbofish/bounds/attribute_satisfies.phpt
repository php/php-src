--TEST--
Turbofish bound check: attribute newInstance accepts an arg that satisfies the bound
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

#[Attribute]
class Bounded<T : Animal> {
    public function __construct() {}
}

#[Bounded::<Dog>]
function f(): void {}

foreach ((new ReflectionFunction('f'))->getAttributes() as $attr) {
    $attr->newInstance();
}
echo "OK\n";
?>
--EXPECT--
OK
