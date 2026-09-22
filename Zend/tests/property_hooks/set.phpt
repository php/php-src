--TEST--
Basic set only property hook
--FILE--
<?php

class Test {
    public $_prop;
    public $prop {
        set { $this->_prop = $value; }
    }
}

$test = new Test;
$test->prop = 42;
var_dump($test->_prop);

try {
    var_dump($test->prop);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(isset($test->prop));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
int(42)
Error: Cannot read from set-only virtual property Test::$prop
Error: Cannot read from set-only virtual property Test::$prop
