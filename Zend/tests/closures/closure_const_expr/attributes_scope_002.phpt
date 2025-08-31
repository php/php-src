--TEST--
Closure in attribute may not access unrelated private variables
--EXTENSIONS--
reflection
--FILE--
<?php

#[Attribute(Attribute::TARGET_CLASS | Attribute::IS_REPEATABLE)]
class Attr {
    public function __construct(public Closure $value) {}
}

#[Attr(static function (E $e) {
    echo $e->secret, PHP_EOL;
})]
class C {
}

class E {
    public function __construct(
        private string $secret,
    ) {}
}

foreach ((new ReflectionClass(C::class))->getAttributes() as $reflectionAttribute) {
    ($reflectionAttribute->newInstance()->value)(new E('secret'));
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access private property E::$secret in %s:%d
Stack trace:
#0 %s(%d): C::{closure:%s:%d}(Object(E))
#1 {main}
  thrown in %s on line %d
