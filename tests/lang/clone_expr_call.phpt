--TEST--
Test call on a clone expression in parentheses
--FILE--
<?php

class Foo {
    public $bar = 'foo';

    public function __clone() {
        $this->bar = 'bar';
    }

    public function bar() {
        return $this->bar;
    }
}

$foo = new Foo;
var_dump((clone $foo)->bar);
var_dump((clone $foo)->bar());

?>
--EXPECT--
string(3) "bar"
string(3) "bar"
