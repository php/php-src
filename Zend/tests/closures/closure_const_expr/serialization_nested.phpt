--TEST--
Serialization of const-expr closures in parameter defaults, hooks and nested functions
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Demo {
    public int $v = 0 {
        #[A(static function () { return 'get-hook'; })]
        get => $this->v;
    }

    public function withDefault($cb = static function () { return 'default'; }) {
        return $cb;
    }

    public function makeClosure() {
        return #[A(static function () { return 'inner'; })] static function () {};
    }
}

class Nested {
    #[A(static function (
        #[A(static function () { return 'deep'; })] $x = null,
    ) { return 'outer'; })]
    public int $p = 0;
}

$roundtrip = static function (Closure $c) {
    $u = unserialize(serialize($c));
    var_dump($u(), preg_replace('/#[0-9a-f]{8}$/', '', (new ReflectionFunction($c))->getConstExprId()));
};

// Attribute on a property hook
$hook = (new ReflectionProperty(Demo::class, 'v'))->getHook(PropertyHookType::Get);
$roundtrip($hook->getAttributes()[0]->getArguments()[0]);

// Parameter default value
$roundtrip((new Demo)->withDefault());

// Attribute on a runtime closure declared in a method body
$runtime = (new Demo)->makeClosure();
$roundtrip((new ReflectionFunction($runtime))->getAttributes()[0]->getArguments()[0]);

// Const-expr closure nested in the parameter attribute of another one
$outer = (new ReflectionProperty(Nested::class, 'p'))->getAttributes()[0]->getArguments()[0];
$inner = (new ReflectionFunction($outer))->getParameters()[0]->getAttributes()[0]->getArguments()[0];
$roundtrip($outer);
$roundtrip($inner);

?>
--EXPECT--
string(8) "get-hook"
string(11) "$v::get()@0"
string(7) "default"
string(15) "withDefault()@0"
string(5) "inner"
string(15) "makeClosure()@0"
string(5) "outer"
string(4) "$p@0"
string(4) "deep"
string(4) "$p@1"
