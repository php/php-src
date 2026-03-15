--TEST--
Generic class: default type parameters
--FILE--
<?php
declare(strict_types=1);

// Single default
class Container<T = string> {
    public function __construct(public T $value) {}
    public function get(): T { return $this->value; }
}

// With explicit type arg — overrides default
$c1 = new Container<int>(42);
echo $c1->get() . "\n";  // 42

// Without type arg — uses default (string)
$c2 = new Container<string>("hello");
echo $c2->get() . "\n";  // hello

// Using default via fewer args
$c3 = new Container("world");
echo $c3->get() . "\n";  // world (inferred as string)

// Multiple params with partial defaults
class Map<K, V = string> {
    /** @var array<int, array{0: mixed, 1: mixed}> */
    private array $items = [];

    public function set(K $key, V $value): void {
        $this->items[] = [$key, $value];
    }

    public function getLastValue(): V {
        $last = end($this->items);
        return $last[1];
    }
}

// Provide both args
$m1 = new Map<int, string>();
$m1->set(1, "one");
echo $m1->getLastValue() . "\n";  // one

// Provide only K, V defaults to string
$m2 = new Map<int>();
$m2->set(1, "default_v");
echo $m2->getLastValue() . "\n";  // default_v

// Type enforcement with default
try {
    $m3 = new Map<int>();
    $m3->set(1, 42);  // Should fail: V=string but got int
} catch (TypeError $e) {
    echo "TypeError: V enforced\n";
}

// Default with constraint
class Repository<T: Countable = ArrayObject> {
    public static function describe(): string {
        return "Repository";
    }
}

$r = new Repository<ArrayObject>();
echo Repository<ArrayObject>::describe() . "\n";

// Reflection
$rc = new ReflectionClass('Map');
$params = $rc->getGenericParameters();
echo "Map params: " . count($params) . "\n";
echo "  K: hasDefault=" . var_export($params[0]->hasDefaultType(), true) . "\n";
echo "  V: hasDefault=" . var_export($params[1]->hasDefaultType(), true) . "\n";
echo "  V default: " . $params[1]->getDefaultType() . "\n";

echo "OK\n";
?>
--EXPECTF--
42
hello
world
one
default_v
TypeError: V enforced
Repository
Map params: 2
  K: hasDefault=false
  V: hasDefault=true
  V default: string
OK
