--TEST--
GH-11781: Constant property assignment
--FILE--
<?php
class Foo {
    public $value;
    public $bar;

    public function __construct() {
        $this->bar = new Bar();
    }
}
class Bar {
    public $value;
}
const FOO = new Foo();
var_dump(FOO->value = 42);
var_dump(FOO->bar->value = 42);
?>
--EXPECT--
int(42)
int(42)
