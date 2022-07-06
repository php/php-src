--TEST--
Attributes AST can be exported.
--INI--
zend.assertions=1
assert.exception=0
assert.warning=1
--FILE--
<?php

assert(0 && ($a = #[A1] #[A2] function ($a, #[A3(1)] $b) { }));

assert(0 && ($a = #[A1(1, 2, 1 + 2)] fn () => 1));

assert(0 && ($a = new #[A1] class() {
    #[A1]#[A2] const FOO = 'foo';
    #[A2] public $x;
    #[A3] function a() { }
}));

assert(0 && ($a = function () {
    #[A1] class Test1 { }
    #[A2] interface Test2 { }
    #[A3] trait Test3 { }
}));

?>
--EXPECTF--
Warning: assert(): assert(0 && ($a = #[A1] #[A2] function ($a, #[A3(1)] $b) {
})) failed in %s on line %d

Warning: assert(): assert(0 && ($a = #[A1(1, 2, 1 + 2)] fn() => 1)) failed in %s on line %d

Warning: assert(): assert(0 && ($a = new #[A1] class {
    #[A1]
    #[A2]
    public const FOO = 'foo';
    #[A2]
    public $x;
    #[A3]
    public function a() {
    }

})) failed in %s on line %d

Warning: assert(): assert(0 && ($a = function () {
    #[A1]
    class Test1 {
    }

    #[A2]
    interface Test2 {
    }

    #[A3]
    trait Test3 {
    }

})) failed in %s on line %d
