--TEST--
Accessor with only implicit get (init-only)
--FILE--
<?php

class Test {
    public int $prop { get; }

    public function __construct(int $prop) {
        $this->prop = $prop;
    }
}

$test = new Test(42);
var_dump($test->prop);
try {
    $test->prop = 24;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop += 24;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(42)
Property Test::$prop is read-only
Property Test::$prop is read-only
