--TEST--
Generic class: Reflection API for generic parameters and type arguments
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
}

class Container<K, out V: Countable> {
    public function get(K $key): V { throw new \Exception("not impl"); }
}

function identity<T>(T $value): T { return $value; }

// Test ReflectionClass::isGeneric()
$rc = new ReflectionClass(Box::class);
var_dump($rc->isGeneric()); // true

$rc2 = new ReflectionClass(stdClass::class);
var_dump($rc2->isGeneric()); // false

// Test ReflectionClass::getGenericParameters()
$params = $rc->getGenericParameters();
echo "Box params count: " . count($params) . "\n";
echo "Box param 0 name: " . $params[0]->getName() . "\n";
echo "Box param 0 constraint: ";
var_dump($params[0]->getConstraint());
echo "Box param 0 invariant: ";
var_dump($params[0]->isInvariant());

// Test Container<K, out V: Countable>
$rc3 = new ReflectionClass(Container::class);
$params3 = $rc3->getGenericParameters();
echo "Container params count: " . count($params3) . "\n";

echo "K name: " . $params3[0]->getName() . "\n";
echo "K invariant: ";
var_dump($params3[0]->isInvariant());
echo "K covariant: ";
var_dump($params3[0]->isCovariant());

echo "V name: " . $params3[1]->getName() . "\n";
echo "V covariant: ";
var_dump($params3[1]->isCovariant());
echo "V contravariant: ";
var_dump($params3[1]->isContravariant());
echo "V constraint: " . $params3[1]->getConstraint()->getName() . "\n";

// Test __toString
echo "K __toString: " . $params3[0] . "\n";
echo "V __toString: " . $params3[1] . "\n";

// Test ReflectionObject::getGenericArguments()
$box = new Box<int>(42);
$ro = new ReflectionObject($box);
var_dump($ro->isGeneric()); // true
$args = $ro->getGenericArguments();
echo "Box<int> args count: " . count($args) . "\n";
echo "Box<int> arg 0: " . $args[0]->getName() . "\n";

// Test non-generic object
$obj = new stdClass();
$ro2 = new ReflectionObject($obj);
$args2 = $ro2->getGenericArguments();
echo "stdClass args count: " . count($args2) . "\n";

// Test generic function reflection
$rf = new ReflectionFunction('identity');
var_dump($rf->isGeneric()); // true
$fparams = $rf->getGenericParameters();
echo "identity params count: " . count($fparams) . "\n";
echo "identity param 0 name: " . $fparams[0]->getName() . "\n";

// Test non-generic function
$rf2 = new ReflectionFunction('strlen');
var_dump($rf2->isGeneric()); // false

echo "OK\n";
?>
--EXPECT--
bool(true)
bool(false)
Box params count: 1
Box param 0 name: T
Box param 0 constraint: NULL
Box param 0 invariant: bool(true)
Container params count: 2
K name: K
K invariant: bool(true)
K covariant: bool(false)
V name: V
V covariant: bool(true)
V contravariant: bool(false)
V constraint: Countable
K __toString: K
V __toString: out V: Countable
bool(true)
Box<int> args count: 1
Box<int> arg 0: int
stdClass args count: 0
bool(true)
identity params count: 1
identity param 0 name: T
bool(false)
OK
