--TEST--
Interaction with magic get/set
--FILE--
<?php

class Test {
    public readonly int $prop;

    public function unsetProp() {
        unset($this->prop);
    }

    public function __get($name) {
        echo __METHOD__, "($name)\n";
        return 1;
    }

    public function __set($name, $value) {
        echo __METHOD__, "($name, $value)\n";
    }

    public function __unset($name) {
        echo __METHOD__, "($name)\n";
    }

    public function __isset($name) {
        echo __METHOD__, "($name)\n";
        return true;
    }
}

$test = new Test;

// The property is in uninitialized state, no magic methods should be invoked.
var_dump(isset($test->prop));
try {
    var_dump($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$test->unsetProp();

var_dump(isset($test->prop));
var_dump($test->prop);
$test->prop = 2;
try {
    unset($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(false)
Typed property Test::$prop must not be accessed before initialization
Cannot initialize readonly property Test::$prop from global scope
Cannot unset readonly property Test::$prop from global scope
Test::__isset(prop)
bool(true)
Test::__get(prop)
int(1)
Test::__set(prop, 2)
Test::__unset(prop)
