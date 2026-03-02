--TEST--
By-ref foreach over readonly property
--FILE--
<?php

class Test {
    public readonly int $prop;

    public function init() {
        $this->prop = 1;
    }
}

$test = new Test;

// Okay, as foreach skips over uninitialized properties.
foreach ($test as &$prop) {}

$test->init();

try {
    foreach ($test as &$prop) {}
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot acquire reference to readonly property Test::$prop
