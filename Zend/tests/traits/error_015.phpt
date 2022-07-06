--TEST--
Trying to add an alias to a trait method where there is another with same name.
Should warn about the conflict.
--FILE--
<?php

trait foo {
    public function test() { return 3; }
}

trait baz {
    public function test() { return 4; }
}

class bar {
    use foo, baz {
        baz::test as zzz;
    }
}

$x = new bar;
var_dump($x->test());

?>
--EXPECTF--
Fatal error: Trait method baz::test has not been applied as bar::test, because of collision with foo::test in %s on line %d
