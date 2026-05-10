--TEST--
Turbofish bound check: attribute newInstance rejects a type argument that violates the bound
--FILE--
<?php
class Animal {}

#[Attribute]
class Bounded<T : Animal> {
    public function __construct() {}
}

#[Bounded::<int>]
function f(): void {}

foreach ((new ReflectionFunction('f'))->getAttributes() as $attr) {
    try {
        $attr->newInstance();
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Type argument 1 to new Bounded does not satisfy the bound Animal on parameter T, int given
