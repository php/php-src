--TEST--
Accessor properties cannot be unset
--FILE--
<?php

class Test {
    public $prop {
        get { return $this->prop; }
        set { $this->prop = $value; }
    }
}

$test = new Test;
$test->prop = 42;
try {
    unset($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop);

?>
--EXPECT--
Cannot unset accessor property Test::$prop
int(42)
