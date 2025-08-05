--TEST--
PFA variation: call
--FILE--
<?php
class Param {

    public function __toString() {
        return __CLASS__;
    }
}

class Foo {
    public function method($a, $b) {
        printf("%s: %s, %s\n", get_called_class(), $a, $b);
    }
}

class Bar extends Foo {

}

$bar = new Bar;
$closure = $bar->method(?, new Param);

$closure(1);

$closure->call(new Foo(), 10);
?>
--EXPECT--
Bar: 1, Param
Foo: 10, Param
