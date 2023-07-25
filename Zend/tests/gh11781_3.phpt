--TEST--
GH-11781: Constant property assignment
--FILE--
<?php
class Foo {
    public $array = [];
}
const FOO = new Foo();
var_dump(FOO->array[] = 42);
?>
--EXPECT--
int(42)
