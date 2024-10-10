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
    $propA->getRawValueWithoutLazyInitialization($obj);
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

var_dump($propB->getRawValueWithoutLazyInitialization($obj));

try {
    var_dump($propC->getRawValueWithoutLazyInitialization($obj));
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

var_dump($propD->getRawValueWithoutLazyInitialization($obj));

print "# Initialized properties\n";

$obj->a = 1;
$obj->b = new stdClass;
$obj->d = 4;

var_dump($propA->getRawValueWithoutLazyInitialization($obj));

var_dump($propB->getRawValueWithoutLazyInitialization($obj));

try {
    var_dump($propC->getRawValueWithoutLazyInitialization($obj));
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

var_dump($propD->getRawValueWithoutLazyInitialization($obj));

try {
    $propStatic->getRawValueWithoutLazyInitialization($obj);
} catch (ReflectionException $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

var_dump($propDynamic->getRawValueWithoutLazyInitialization($obj));

print "# Unset properties and __get()\n";

$obj = new D();
unset($obj->a);
unset($obj->b);

var_dump($propA->getRawValueWithoutLazyInitialization($obj));

var_dump($propB->getRawValueWithoutLazyInitialization($obj));

print "# References\n";

$obj = new C();
$obj->b = &$obj;

var_dump($propB->getRawValueWithoutLazyInitialization($obj));

print "# Internal class\n";

$reflector = new ReflectionClass(Exception::class);
$propMessage = $reflector->getProperty('message');

$obj = new Exception('hello');

var_dump($propMessage->getRawValueWithoutLazyInitialization($obj));

?>
--EXPECTF--
# Non initialized properties
Error: Typed property C::$a must not be accessed before initialization
NULL
Error: Must not read from virtual property C::$c
NULL
# Initialized properties
int(1)
object(stdClass)#%d (0) {
}
Error: Must not read from virtual property C::$c
int(4)
ReflectionException: May not use getRawValueWithoutLazyInitialization on static properties

Warning: Undefined property: C::$dynamic in %s on line %d
NULL
# Unset properties and __get()
int(97)
int(98)
# References
object(C)#%d (2) {
  ["a"]=>
  uninitialized(int)
  ["b"]=>
  *RECURSION*
  ["d"]=>
  NULL
}
# Internal class
string(5) "hello"
