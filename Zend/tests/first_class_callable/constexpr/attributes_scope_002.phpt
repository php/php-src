--TEST--
FCC in attribute may not access unrelated private methods
--EXTENSIONS--
reflection
--FILE--
<?php

#[Attribute(Attribute::TARGET_CLASS | Attribute::IS_REPEATABLE)]
class Attr {
    public function __construct(public Closure $value) {}
}

class E {
    private static function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

#[Attr(E::myMethod(...))]
class C {
}

foreach ((new ReflectionClass(C::class))->getAttributes() as $reflectionAttribute) {
    ($reflectionAttribute->newInstance()->value)('abc');
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private method E::myMethod() from scope C in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d
