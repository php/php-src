--TEST--
Basic auto-generated accessors
--FILE--
<?php

class Test {
    public $prop { get; private set; }

    public function __construct() {
        $this->prop = 42;
        $this->prop = 24;
    }
}

$test = new Test;
try {
    $test->prop = 12;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop);
var_dump(isset($test->prop));
try {
    unset($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to private accessor Test::$prop::set() from global scope
int(24)
bool(true)
Cannot unset accessor property Test::$prop
