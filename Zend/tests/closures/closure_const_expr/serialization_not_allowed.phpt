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
}

?>
--EXPECT--
runtime closure: Serialization of 'Closure' is not allowed
static runtime closure: Serialization of 'Closure' is not allowed
arrow function: Serialization of 'Closure' is not allowed
runtime function callable: Serialization of 'Closure' is not allowed
runtime method callable: Serialization of 'Closure' is not allowed
bound method callable: Serialization of 'Closure' is not allowed
private method callable: Serialization of 'Closure' is not allowed
__callStatic trampoline: Serialization of 'Closure' is not allowed
class constant value: Serialization of 'Closure' is not allowed
property default: Serialization of 'Closure' is not allowed
rebound scope: Serialization of 'Closure' is not allowed
free function attribute: Serialization of 'Closure' is not allowed
anonymous class attribute: Serialization of 'Closure' is not allowed
