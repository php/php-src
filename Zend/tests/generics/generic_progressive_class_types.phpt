--TEST--
Generic class: progressive inference with class/object types
--FILE--
<?php
declare(strict_types=1);

class Animal {
    public string $name;
    public function __construct(string $name) { $this->name = $name; }
}

class Dog extends Animal {}
class Cat extends Animal {}
class Fish extends Animal {}

class Cage<T> {
    /** @var T[] */
    private array $occupants = [];
    public function add(T $occupant): void { $this->occupants[] = $occupant; }
    public function getAll(): array { return $this->occupants; }
}

// === Progressive with object types ===
echo "--- Object types ---\n";
$cage = new Cage();
$cage->add(new Dog("Rex"));
echo "Added Dog: OK\n";

$cage->add(new Cat("Whiskers"));
echo "Added Cat: OK\n";

// Still progressive, max is mixed — any type works
$cage->add(new Fish("Nemo"));
echo "Added Fish: OK (max still mixed)\n";

echo "Count: " . count($cage->getAll()) . "\n";

// === Narrowing with class type ===
echo "--- Narrowing with class constraint ---\n";

$cage2 = new Cage();
$cage2->add(new Dog("Buddy"));
echo "Added Dog: OK\n";

function acceptAnimalCage(Cage<Animal> $c): void {
    echo "Accepted as Cage<Animal>\n";
}

// Narrow max to Animal — Dog is a subtype of Animal, so min ⊆ max
acceptAnimalCage($cage2);

// Dog is still allowed (subclass of Animal)
$cage2->add(new Dog("Max"));
echo "After narrowing: added Dog OK\n";

// Cat is also a subclass of Animal — should work
$cage2->add(new Cat("Luna"));
echo "After narrowing: added Cat OK\n";

// int is NOT an Animal — should fail
try {
    $cage2->add(42);
    echo "FAIL: int should have been rejected\n";
} catch (TypeError $e) {
    echo "After narrowing: int rejected (OK)\n";
}

// string is NOT an Animal — should fail
try {
    $cage2->add("not an animal");
    echo "FAIL: string should have been rejected\n";
} catch (TypeError $e) {
    echo "After narrowing: string rejected (OK)\n";
}

// === Narrowing to scalar after objects ===
echo "--- Mixed scalar + object then narrowing ---\n";

class Wrapper<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function set(T $value): void { $this->value = $value; }
}

// Wrapper with no ctor param type that matches T (ctor infers to int)
$w = new Wrapper(42);
echo "Wrapper inferred type: int (frozen)\n";

try {
    $w->set("text");
    echo "FAIL: should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "Wrapper<int>: string rejected (OK)\n";
}

echo "Done.\n";
?>
--EXPECT--
--- Object types ---
Added Dog: OK
Added Cat: OK
Added Fish: OK (max still mixed)
Count: 3
--- Narrowing with class constraint ---
Added Dog: OK
Accepted as Cage<Animal>
After narrowing: added Dog OK
After narrowing: added Cat OK
After narrowing: int rejected (OK)
After narrowing: string rejected (OK)
--- Mixed scalar + object then narrowing ---
Wrapper inferred type: int (frozen)
Wrapper<int>: string rejected (OK)
Done.
