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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($test->prop));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(42)
Property Test::$prop is write-only
Property Test::$prop is write-only
