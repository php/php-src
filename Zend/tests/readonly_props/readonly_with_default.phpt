--TEST--
Readonly property with default value
--FILE--
<?php

class Test {
    public readonly int $prop = 1;
}

$test = new Test;
try {
    $test->prop = 2;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Fatal error: Readonly property Test::$prop cannot have default value in %s on line %d
