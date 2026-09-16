--TEST--
Bug #55719 (Argument restriction should come with a more specific error message)
--FILE--
<?php
class Foo {
}

Abstract Class Base {
    public function test(Foo $foo, array $bar, $option = NULL, $extra = "lllllllllllllllllllllllllllllllllllllllllllllllllll") {
    }
}

class Sub extends Base {
    public function test() {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of Sub::test() must be compatible with Base::test(Foo $foo, array $bar, $option = null, $extra = 'llllllllll...') in %s on line %d
