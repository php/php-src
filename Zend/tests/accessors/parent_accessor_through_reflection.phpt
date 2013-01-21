--TEST--
The parent accessor can be accessed through Reflection
--FILE--
<?php

class Test {
    public $foo {
        get { echo __METHOD__."() = "; return $this->foo; }
        set { echo __METHOD__."($value)".PHP_EOL; $this->foo = $value; }
    }
}

class Test2 extends Test {
    public $foo {
        get {
            echo __METHOD__."() -> ";
            return (new ReflectionProperty(get_parent_class(), 'foo'))->getValue($this);
        }
        set {
            echo __METHOD__."($value) -> ";
            (new ReflectionProperty(get_parent_class(), 'foo'))->setValue($this, $value);
        }
    }
}

$test = new Test2;
$test->foo = 'value';
var_dump($test->foo);
?>
===DONE===
--EXPECTF--
Test2::$foo->set(value) -> Test::$foo->set(value)
Test2::$foo->get() -> Test::$foo->get() = string(%d) "value"
===DONE===