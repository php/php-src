--TEST--
Test typed properties int must not be allowed to overflow
--FILE--
<?php
class Foo {
    public int $bar = PHP_INT_MAX;

    public function inc() {
        return ++$this->bar;
    }
}

$foo = new Foo();

try {
    $foo->inc();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot increment property Foo::$bar of type int past its maximal value
