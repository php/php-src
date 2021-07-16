--TEST--
Unset readonly property
--FILE--
<?php

class Test {
    public readonly int $prop;

    public function __construct(int $prop) {
        $this->prop = $prop;
    }
}

$test = new Test(1);
try {
    unset($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

class Test2 {
    public readonly int $prop;

    public function __construct() {
        unset($this->prop); // Unset uninitialized.
        unset($this->prop); // Unset unset.
    }

    public function __get($name) {
        // Lazy init.
        echo __METHOD__, "\n";
        $this->prop = 1;
        return $this->prop;
    }
}

$test = new Test2;
var_dump($test->prop); // Call __get.
var_dump($test->prop); // Don't call __get.
try {
    unset($test->prop); // Unset initialized, illegal.
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

class Test3 {
    public readonly int $prop;
}

$test = new Test3;
try {
    unset($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot unset readonly property Test::$prop
Test2::__get
int(1)
int(1)
Cannot unset readonly property Test2::$prop
Cannot unset readonly property Test3::$prop from global scope
