--TEST--
Call property hooks by name
--FILE--
<?php

class Test {
    public $prop {
        get { echo "get called\n"; }
        set { echo "set called with $value\n"; }
    }
}

$test = new Test;
try {
    $test->{'$prop::get'}();
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->{'$prop::set'}('foo');
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to undefined method Test::$prop::get()
Call to undefined method Test::$prop::set()
