--TEST--
Exception thrown by __get() during =& assignment
--FILE--
<?php

class Test {
    private $x;
    public function &__get($name) {
        throw new Exception("Foobar");
    }
}

$test = new Test;
$y = 5;
try {
    $test->x =& $y;
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Foobar
