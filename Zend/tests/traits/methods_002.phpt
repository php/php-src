--TEST--
Testing collision with magic methods
--FILE--
<?php

trait foo {
    public function __clone() {
        var_dump(__FUNCTION__);
    }
}

trait baz {
    public function __clone() {
        var_dump(__FUNCTION__);
    }
}

class bar {
    use foo;
    use baz;
}

$o = new bar;
var_dump(clone $o);

?>
--EXPECTF--
Fatal error: Trait method baz::__clone has not been applied as bar::__clone, because of collision with foo::__clone in %s on line %d
