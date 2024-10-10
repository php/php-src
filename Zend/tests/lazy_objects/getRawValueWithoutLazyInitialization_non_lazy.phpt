--TEST--
Lazy objects: getRawValueWithoutLazyInitialization() behaves like getRawValue() on non-lazy objects
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public static $static = 'static';

    public int $a;
    public $b;
    public $c {
        get { return 'c'; }
    }
    public $d {
        get { return $this->d; }
        set($value) { $this->d = $value; }
    }
}

class D extends C {
    public function __get($name) {
        return ord($name);
    }
}

$reflector = new ReflectionClass(C::class);
$propA = $reflector->getProperty('a');
$propB = $reflector->getProperty('b');
$propC = $reflector->getProperty('c');
$propD = $reflector->getProperty('d');
$propStatic = $reflector->getProperty('static');

$obj = new C();
$obj->dynamic = 1;
$propDynamic = new ReflectionProperty($obj, 'dynamic');

$obj = new C();

print "# Non initialized properties\n";

try {
    $propA->getRawValueWithoutLazyInitialization($obj, $isLazy);
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$isLazy = null;
var_dump($propB->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

try {
    var_dump($propC->getRawValueWithoutLazyInitialization($obj, $isLazy));
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$isLazy = null;
var_dump($propD->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

print "# Initialized properties\n";

$obj->a = 1;
$obj->b = new stdClass;
$obj->d = 4;

$isLazy = null;
var_dump($propA->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

$isLazy = null;
var_dump($propB->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

try {
    var_dump($propC->getRawValueWithoutLazyInitialization($obj, $isLazy));
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$isLazy = null;
var_dump($propD->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

try {
    $propStatic->getRawValueWithoutLazyInitialization($obj, $isLazy);
} catch (ReflectionException $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$isLazy = null;
var_dump($propDynamic->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

print "# Unset properties and __get()\n";

$obj = new D();
unset($obj->a);
unset($obj->b);

$isLazy = null;
var_dump($propA->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

$isLazy = null;
var_dump($propB->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

print "# Internal class\n";

$reflector = new ReflectionClass(Exception::class);
$propMessage = $reflector->getProperty('message');

$obj = new Exception('hello');

$isLazy = null;
var_dump($propMessage->getRawValueWithoutLazyInitialization($obj, $isLazy));
var_dump($isLazy);

?>
--EXPECTF--
# Non initialized properties
Error: Typed property C::$a must not be accessed before initialization
NULL
bool(false)
Error: Must not read from virtual property C::$c
NULL
bool(false)
# Initialized properties
int(1)
bool(false)
object(stdClass)#%d (0) {
}
bool(false)
Error: Must not read from virtual property C::$c
int(4)
bool(false)
ReflectionException: May not use getRawValueWithoutLazyInitialization on static properties

Warning: Undefined property: C::$dynamic in %s on line %d
NULL
bool(false)
# Unset properties and __get()
int(97)
bool(false)
int(98)
bool(false)
# Internal class
string(5) "hello"
bool(false)
