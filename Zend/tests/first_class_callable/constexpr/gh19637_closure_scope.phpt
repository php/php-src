--TEST--
FCC in attribute sets Closure scope.
--EXTENSIONS--
reflection
--FILE--
<?php

#[Attribute(Attribute::TARGET_CLASS)]
class Attr {
    public function __construct(public array $value) {}
}

class F {
    public static function foreign() {}
}

class G extends F { }

#[Attr([
    F::foreign(...),
    G::foreign(...),
    self::myMethod(...),
    strrev(...),
])]
class C {
    private static function myMethod(string $foo) {
        return "XXX";
    }

    public static function foo() {
        foreach ([
            F::foreign(...),
            G::foreign(...),
            self::myMethod(...),
            strrev(...),
        ] as $fn) {
            $r = new \ReflectionFunction($fn);
            var_dump($r->getClosureCalledClass());
            var_dump($r->getClosureScopeClass());
        }
    }
}
 
foreach ((new ReflectionClass(C::class))->getAttributes() as $reflectionAttribute) {
    foreach ($reflectionAttribute->newInstance()->value as $fn) {
        $r = new \ReflectionFunction($fn);
        var_dump($r->getClosureCalledClass());
        var_dump($r->getClosureScopeClass());
    }
}
echo "=======\n";
C::foo();

?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "F"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "F"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "G"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "F"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "C"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "C"
}
NULL
NULL
=======
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "F"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "F"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "G"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "F"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "C"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "C"
}
NULL
NULL
