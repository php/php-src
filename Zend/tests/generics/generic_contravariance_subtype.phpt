--TEST--
Generic class: contravariant subtyping (Consumer<Animal> assignable to Consumer<Dog>)
--FILE--
<?php
declare(strict_types=1);

class Animal {
    public string $name;
    public function __construct(string $name) { $this->name = $name; }
}
class Dog extends Animal {}

class Consumer<in T> {
    private mixed $callback;
    public function __construct(mixed $callback) { $this->callback = $callback; }
    public function consume(T $item): void {
        echo ($this->callback)($item) . "\n";
    }
}

function feedDog(Consumer<Dog> $consumer): void {
    $consumer->consume(new Dog("Rex"));
}

// Contravariant: Consumer<Animal> should be assignable to Consumer<Dog>
// because a consumer of Animals can consume any Dog (Dog is an Animal)
$animalConsumer = new Consumer<Animal>(function(Animal $a) { return "Consumed: " . $a->name; });
feedDog($animalConsumer);

echo "OK\n";
?>
--EXPECT--
Consumed: Rex
OK
