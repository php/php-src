--TEST--
Closures that are not addressable by a declaration-site reference still refuse to serialize
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Demo {
    // Class constant values and property defaults are evaluated in place,
    // so closures declared there are not addressable.
    public const BAD = static function () {};
    public $bad = static function () {};

    #[A(static function () { return 'ok'; })]
    public int $good = 0;

    public function boundMethod() {}
    private static function privateMethod() {}
    public static function privateFcc(): Closure { return self::privateMethod(...); }
    public static function __callStatic($name, $args) {}
}

#[A(static function () {})]
function freeFunction() {}

$anon = new #[A(static function () {})] class {};

$attrClosure = (new ReflectionProperty(Demo::class, 'good'))->getAttributes()[0]->getArguments()[0];

$cases = [
    'runtime closure' => function () {},
    'static runtime closure' => static function () {},
    'arrow function' => fn () => 1,
    'runtime function callable' => strlen(...),
    'runtime method callable' => Demo::privateFcc(...),
    'bound method callable' => (new Demo)->boundMethod(...),
    'private method callable' => Demo::privateFcc(),
    '__callStatic trampoline' => Demo::someUndefinedMethod(...),
    'class constant value' => Demo::BAD,
    'property default' => (new Demo)->bad,
    'rebound scope' => Closure::bind($attrClosure, null, A::class),
    'free function attribute' => (new ReflectionFunction('freeFunction'))->getAttributes()[0]->getArguments()[0],
    'anonymous class attribute' => (new ReflectionClass($anon))->getAttributes()[0]->getArguments()[0],
];

foreach ($cases as $name => $closure) {
    try {
        serialize($closure);
        echo "$name: serialized!?\n";
    } catch (Exception $e) {
        echo "$name: {$e->getMessage()}\n";
    }
    var_dump((new ReflectionFunction($closure))->getConstExprId());
}

?>
--EXPECT--
runtime closure: Serialization of 'Closure' is not allowed
NULL
static runtime closure: Serialization of 'Closure' is not allowed
NULL
arrow function: Serialization of 'Closure' is not allowed
NULL
runtime function callable: Serialization of 'Closure' is not allowed
NULL
runtime method callable: Serialization of 'Closure' is not allowed
NULL
bound method callable: Serialization of 'Closure' is not allowed
NULL
private method callable: Serialization of 'Closure' is not allowed
NULL
__callStatic trampoline: Serialization of 'Closure' is not allowed
NULL
class constant value: Serialization of 'Closure' is not allowed
NULL
property default: Serialization of 'Closure' is not allowed
NULL
rebound scope: Serialization of 'Closure' is not allowed
NULL
free function attribute: Serialization of 'Closure' is not allowed
NULL
anonymous class attribute: Serialization of 'Closure' is not allowed
NULL
