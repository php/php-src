--TEST--
ZE2 Late Static Binding within hooks/magic methods
--FILE--
<?php

class TestChild extends TestParent {

    public static function who() {
        echo __CLASS__."\n";
    }
}

class TestParent {

    public function __get($var) {
        static::who();
    }

    public function __set($var, $val) {
        static::who();
    }

    public function __call($name, $args) {
        static::who();
    }

    public static function who() {
        echo __CLASS__."\n";
    }
}
$o = new TestChild;
$o->test();
$o->a = "b";
echo $o->a;
?>
--EXPECT--
TestChild
TestChild
TestChild
