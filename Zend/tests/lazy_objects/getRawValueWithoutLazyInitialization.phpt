--TEST--
Lazy objects: getRawValueWithoutLazyInitialization() fetches raw value without triggering initialization
--FILE--
<?php

class C {
    public int $a;
    public $b = 2;
}

$reflector = new ReflectionClass(C::class);
$propA = $reflector->getProperty('a');
$propB = $reflector->getProperty('b');

$obj = $reflector->newLazyGhost(function () {
    throw new \Exception('initializer');
});

print "# Ghost: Lazy properties\n";

$isLazy = null;
var_dump($propA->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

$isLazy = null;
var_dump($propB->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

print "# Ghost: Initialized properties\n";

$propA->setRawValueWithoutLazyInitialization($obj, 1);

$isLazy = null;
var_dump($propA->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

$propB->skipLazyInitialization($obj);

$isLazy = null;
var_dump($propB->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

$obj = $reflector->newLazyProxy(function () {
    throw new \Exception('initializer');
});

print "# Proxy: Lazy properties\n";

$isLazy = null;
var_dump($propA->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

$isLazy = null;
var_dump($propB->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

print "# Proxy: Initialized properties\n";

$propA->setRawValueWithoutLazyInitialization($obj, 1);

$isLazy = null;
var_dump($propA->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

$propB->skipLazyInitialization($obj);

$isLazy = null;
var_dump($propB->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

$obj = $reflector->newLazyProxy(function () {
    return new C();
});
$reflector->initializeLazyObject($obj);

print "# Initialized Proxy\n";

try {
    $propA->getRawValueWithoutLazyInitialization($obj, $isLazy);
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$isLazy = null;
var_dump($propB->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

?>
--EXPECT--
# Ghost: Lazy properties
NULL
bool(true)
NULL
bool(true)
# Ghost: Initialized properties
int(1)
bool(false)
int(2)
bool(false)
# Proxy: Lazy properties
NULL
bool(true)
NULL
bool(true)
# Proxy: Initialized properties
int(1)
bool(false)
int(2)
bool(false)
# Initialized Proxy
Error: Typed property C::$a must not be accessed before initialization
int(2)
bool(false)
