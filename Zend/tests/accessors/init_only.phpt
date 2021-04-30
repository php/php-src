--TEST--
Accessor with only implicit get (init-only)
--FILE--
<?php

class Test {
    public int $prop { get; }
    public $prop2 { get; }
    public $prop3 { get; }

    public function __construct(int $prop, $prop2) {
        $this->prop = $prop;
        $this->prop2 = $prop2;
    }
}

$test = new Test(1, 2);
var_dump($test->prop);
try {
    $test->prop = 3;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop += 4;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo "\n";

var_dump($test->prop2);
try {
    $test->prop2 = 5;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo "\n";

try {
    var_dump($test->prop3);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
$test->prop3 = 6;
var_dump($test->prop3);
try {
    $test->prop3 = 7;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(1)
Property Test::$prop is read-only
Property Test::$prop is read-only

int(2)
Property Test::$prop2 is read-only

Property Test::$prop3 must not be accessed before initialization
int(6)
Property Test::$prop3 is read-only
