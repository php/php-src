--TEST--
Interaction of inaccessible accessors with magic methods
--FILE--
<?php

class A {
    public $prop {
        private get { echo __METHOD__, "\n"; }
        private set { echo __METHOD__, "\n"; }
    }
}

class B extends A {
    public function __get($name) {
        echo __METHOD__, "($name)\n";
        try {
            $this->$name;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
    public function __set($name, $value) {
        echo __METHOD__, "($name, $value)\n";
        try {
            $this->$name = $value;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
    public function __isset($name) {
        echo __METHOD__, "($name)\n";
        try {
            var_dump(isset($this->$name));
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
    public function __unset($name) {
        echo __METHOD__, "($name)\n";
        try {
            unset($this->$name);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$b = new B;
$b->prop;
isset($b->prop);
$b->prop = 1;
unset($b->prop);

?>
--EXPECT--
B::__get(prop)
Call to private accessor A::$prop::get() from scope B
B::__isset(prop)
bool(false)
B::__set(prop, 1)
Call to private accessor A::$prop::set() from scope B
B::__unset(prop)
Cannot unset accessor property B::$prop
