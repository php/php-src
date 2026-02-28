--TEST--
Generic trait: Reflection API for generic trait parameters
--FILE--
<?php
declare(strict_types=1);

trait Cacheable<T> {
    private T $cached;
    public function cache(T $value): void { $this->cached = $value; }
    public function getCached(): T { return $this->cached; }
}

trait Transform<In, out Out> {
    abstract public function transform(In $input): Out;
}

class UserCache {
    use Cacheable<string>;
}

// Reflect the generic trait itself
$rt = new ReflectionClass('Cacheable');
var_dump($rt->isGeneric()); // true
$params = $rt->getGenericParameters();
echo "Cacheable params: " . count($params) . "\n";
echo "  T: " . $params[0]->getName() . "\n";
echo "  T invariant: ";
var_dump($params[0]->isInvariant());

// Reflect multi-param trait with variance
$rt2 = new ReflectionClass('Transform');
var_dump($rt2->isGeneric()); // true
$params2 = $rt2->getGenericParameters();
echo "Transform params: " . count($params2) . "\n";
echo "  In: " . $params2[0]->getName() . " invariant=";
var_dump($params2[0]->isInvariant());
echo "  Out: " . $params2[1]->getName() . " covariant=";
var_dump($params2[1]->isCovariant());

// Reflect the class using the generic trait
$rc = new ReflectionClass('UserCache');
var_dump($rc->isGeneric()); // false (UserCache has no own type params)
echo "UserCache own params: " . count($rc->getGenericParameters()) . "\n";
echo "UserCache traits: " . implode(', ', $rc->getTraitNames()) . "\n";

echo "OK\n";
?>
--EXPECT--
bool(true)
Cacheable params: 1
  T: T
  T invariant: bool(true)
bool(true)
Transform params: 2
  In: In invariant=bool(true)
  Out: Out covariant=bool(true)
bool(false)
UserCache own params: 0
UserCache traits: Cacheable
OK
