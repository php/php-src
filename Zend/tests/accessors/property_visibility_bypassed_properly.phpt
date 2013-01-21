--TEST--
ZE2 Visibility of property is ignored when involving an accessor (accessor visibility checked)
--FILE--
<?php

class Foo {
    protected $bar {
        public get { return 'test'; }
    }
}

$foo = new Foo();
var_dump($foo->bar);

class Foo2 {
    private $bar {
        public get { return 'test2'; }
    }
}

$foo2 = new Foo2();
var_dump($foo2->bar);

?>
===DONE===
--EXPECTF--
string(%d) "test"
string(%d) "test2"
===DONE===