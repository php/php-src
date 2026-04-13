--TEST--
Generic function: inference with object and class types
--DESCRIPTION--
Tests generic function type inference when arguments are objects.
The inference extracts the class name from the zval and uses instanceof
to verify return type compatibility.
--FILE--
<?php
declare(strict_types=1);

class Animal {
    public string $name;
    public function __construct(string $name) { $this->name = $name; }
}
class Dog extends Animal {}
class Cat extends Animal {}

// 1. Basic object inference
function get_name<T>(T $obj): string {
    if ($obj instanceof Animal) return $obj->name;
    return "unknown";
}

echo "1. " . get_name(new Dog("Rex")) . "\n";
echo "1. " . get_name(new Cat("Whiskers")) . "\n";

// 2. Object identity — infer T=Dog, enforce Dog return
function clone_animal<T>(T $animal): T {
    return $animal; // same object back
}

$d = clone_animal(new Dog("Rex"));
echo "2. " . get_class($d) . "\n";

// 3. Wrong return type: returning sibling class (Cat is not Dog)
function wrong_clone<T>(T $animal): T {
    return new Cat("Wrong"); // Cat is not Dog
}

try {
    wrong_clone(new Dog("Rex")); // T=Dog, returning Cat
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "3. sibling TypeError OK\n";
}

// 4. Wrong return type: returning array instead of object
function bad_clone<T>(T $animal): T {
    return [1, 2, 3]; // array is never an object
}

try {
    bad_clone(new Dog("Rex")); // T=Dog, returning array
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "4. array TypeError OK\n";
}

// 5. Object + scalar mix
function describe<A, B>(A $obj, B $label): string {
    if ($obj instanceof Animal) return $obj->name . ": " . $label;
    return "?: " . $label;
}

echo "5. " . describe(new Dog("Rex"), "good boy") . "\n";

// 6. stdClass inference
function get_prop<T>(T $obj): string {
    if ($obj instanceof stdClass) return $obj->x ?? "none";
    return "not stdClass";
}

$o = new stdClass();
$o->x = "hello";
echo "6. " . get_prop($o) . "\n";

// 7. Returning subclass is OK (Dog is an Animal)
function upcast<T>(T $animal): T {
    return $animal;
}

$dog = upcast(new Dog("Buddy"));
echo "7. " . get_class($dog) . ": " . $dog->name . "\n";

echo "Done.\n";
?>
--EXPECT--
1. Rex
1. Whiskers
2. Dog
3. sibling TypeError OK
4. array TypeError OK
5. Rex: good boy
6. hello
7. Dog: Buddy
Done.
