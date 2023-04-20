--TEST--
Interaction of inaccessible property hooks with magic methods
--FILE--
<?php

class A {
    public $prop {
        get {
            echo __METHOD__, "\n";
            return 'prop';
        }
        set { echo __METHOD__, "\n"; }
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
        echo "Never reached\n";
    }
}

$b = new B;
$b->prop;
var_dump(isset($b->prop));
$b->prop = 1;
try {
    unset($b->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
A::$prop::get
A::$prop::get
bool(true)
A::$prop::set
Cannot unset hooked property B::$prop
