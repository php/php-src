--TEST--
Bug #63399 (ReflectionClass::getTraitAliases() incorrectly resolves traitnames)
--FILE--
<?php
trait Trait1 {
        public function run() {}
        public function say() {}
}

trait Trait2 {
        public function run() {}
        public function say() {}
}

class MyClass
{
    use Trait1, Trait2 {
        Trait1::run as execute;
        Trait1::say insteadof Trait2;
        Trait2::run insteadof Trait1;
        Trait2::say as talk;
    }
}

$ref = new ReflectionClass('MyClass');

print_r($ref->getTraitAliases());
print_r($ref->getTraits());

?>
--EXPECT--
Array
(
    [execute] => Trait1::run
    [talk] => Trait2::say
)
Array
(
    [Trait1] => ReflectionClass Object
        (
            [name] => Trait1
        )

    [Trait2] => ReflectionClass Object
        (
            [name] => Trait2
        )

)
