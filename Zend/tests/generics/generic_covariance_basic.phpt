--TEST--
Generic class: covariance (out) annotation and compile-time validation
--FILE--
<?php
declare(strict_types=1);

class Animal {}
class Dog extends Animal {}
class Cat extends Animal {}

// Covariant type parameter: can only appear in return positions
class ReadOnlyBox<out T> {
    private mixed $value;
    public function __construct(mixed $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

$dogBox = new ReadOnlyBox<Dog>(new Dog());
$dog = $dogBox->get();
echo get_class($dog) . "\n";

echo "OK\n";
?>
--EXPECT--
Dog
OK
