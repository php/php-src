--TEST--
Property access errors should be thrown for overloaded properties protected by recursion guards
--FILE--
<?php

function setProp($obj) {
    $obj->prop = 42;
}

function getProp($obj) {
    var_dump($obj->prop);
}

function unsetProp($obj) {
    unset($obj->prop);
}

class Test {
    private $prop;

    public function __get($k) {
        getProp($this);
    }

    public function __set($k, $v) {
        setProp($this);
    }

    public function __unset($k) {
        unsetProp($this);
    }
}

$test = new Test;
try {
    $test->prop = "bar";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot access private property Test::$prop
Cannot access private property Test::$prop
Cannot access private property Test::$prop
