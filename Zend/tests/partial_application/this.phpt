--TEST--
PFA $this
--FILE--
<?php
class Foo {
    public function method($a, $b) {
        return $this;
    }
}

$foo = new Foo;

$bar = $foo->method(new stdClass, ...);

$baz = $bar(new stdClass, ...);

var_dump($foo === $baz());
?>
--EXPECT--
bool(true)
