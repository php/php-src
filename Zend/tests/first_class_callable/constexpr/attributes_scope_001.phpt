--TEST--
FCC in attribute may access private methods
--EXTENSIONS--
reflection
--FILE--
<?php

#[Attribute(Attribute::TARGET_CLASS | Attribute::IS_REPEATABLE)]
class Attr {
    public function __construct(public Closure $value) {}
}

#[Attr(C::myMethod(...))]
class C {
    private static function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

foreach ((new ReflectionClass(C::class))->getAttributes() as $reflectionAttribute) {
    ($reflectionAttribute->newInstance()->value)('abc');
}

?>
--EXPECT--
Called C::myMethod
string(3) "abc"
