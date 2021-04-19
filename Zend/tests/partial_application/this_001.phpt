--TEST--
Partial application this
--FILE--
<?php
class Foo {
    public function method() {
        return $this;
    }
}

$foo = new Foo;

$bar = $foo->method(...);

$baz = $bar(...);

var_dump($baz());
?>
--EXPECTF--
object(Foo)#%d (0) {
}

