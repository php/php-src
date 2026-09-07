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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $test->{'$prop::set'}('foo');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Call to undefined method Test::$prop::get()
Error: Call to undefined method Test::$prop::set()
