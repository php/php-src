--TEST--
Can define die and exit as class methods, constants and property
--FILE--
<?php

class Foo {
    public $exit;
    public $die;

    const die = 5;
    const exit = 10;

    public function exit() {
        return 20;
    }

    public function die() {
        return 15;
    }
}

var_dump(Foo::die);
var_dump(Foo::exit);
$o = new Foo();
var_dump($o->exit);
var_dump($o->die);
var_dump($o->exit());
var_dump($o->die());

?>
--EXPECT--
int(5)
int(10)
NULL
NULL
int(20)
int(15)
