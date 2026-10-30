--TEST--
Trait hook scope is changed
--FILE--
<?php

trait Foo {
    private string $x = 'bar';

    public function getBar() {
        return $this->x;
    }

    public string $bar {
        get => $this->x;
    }
}

class A {
    use Foo;
}

$a = new A();
var_dump($a->getBar());
var_dump($a->bar);

?>
--EXPECT--
string(3) "bar"
string(3) "bar"
