--TEST--
Attributes AST can be exported.
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(0 && ($a = #[A1] #[A2] function ($a, #[A3(1)] $b) { }));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

try {
    assert(0 && ($a = #[A1(1, 2, 1 + 2)] fn () => 1));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

try {
assert(0 && ($a = new #[A1] class() {
    #[A1]#[A2] const FOO = 'foo';
    #[A2] public $x;
    #[A3] function a() { }
}));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

try {
assert(0 && ($a = function () {
    #[A1] class Test1 { }
    #[A2] interface Test2 { }
    #[A3] trait Test3 { }
}));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

?>
--EXPECT--
assert(): assert(0 && ($a = #[A1] #[A2] function ($a, #[A3(1)] $b) {
})) failed
assert(): assert(0 && ($a = #[A1(1, 2, 1 + 2)] fn() => 1)) failed
assert(): assert(0 && ($a = new #[A1] class {
    #[A1]
    #[A2]
    public const FOO = 'foo';
    #[A2]
    public $x;
    #[A3]
    public function a() {
    }

})) failed
assert(): assert(0 && ($a = function () {
    #[A1]
    class Test1 {
    }

    #[A2]
    interface Test2 {
    }

    #[A3]
    trait Test3 {
    }

})) failed
