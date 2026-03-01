--TEST--
Generic class: covariant subtyping (ReadOnlyBox<Dog> assignable to ReadOnlyBox<Animal>)
--FILE--
<?php
declare(strict_types=1);

class Animal {
    public string $name;
    public function __construct(string $name) { $this->name = $name; }
}
class Dog extends Animal {}
class Cat extends Animal {}

class ReadOnlyBox<out T> {
    private mixed $value;
    public function __construct(mixed $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

function processAnimalBox(ReadOnlyBox<Animal> $box): string {
    return $box->get()->name;
}

// Covariant: ReadOnlyBox<Dog> should be assignable to ReadOnlyBox<Animal>
$dogBox = new ReadOnlyBox<Dog>(new Dog("Rex"));
echo processAnimalBox($dogBox) . "\n";

$catBox = new ReadOnlyBox<Cat>(new Cat("Whiskers"));
echo processAnimalBox($catBox) . "\n";

echo "OK\n";
?>
--EXPECT--
Rex
Whiskers
OK
