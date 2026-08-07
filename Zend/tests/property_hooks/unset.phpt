--TEST--
Hooked properties cannot be unset
--FILE--
<?php

class Test {
    public $prop {
        get { return $this->prop; }
        set { $this->prop = $value; }
    }

    public function __unset($name) {
        echo "Never reached\n";
    }
}

$test = new Test;
$test->prop = 42;
try {
    unset($test->prop);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->prop);

?>
--EXPECT--
Error: Cannot unset hooked property Test::$prop
int(42)
