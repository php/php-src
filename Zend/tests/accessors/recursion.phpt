--TEST--
Recursion behavior of accessors
--FILE--
<?php

class Test {
    public int $prop {
        get { return $this->prop * 2; }
        set { $this->prop = $value * 2; }
    }

    // Edge-case where recursion happens via isset().
    public int $prop2 {
        get { return isset($this->prop2); }
        set { }
    }

    public int $prop3 {
        get { return $this->prop3 * 2; }
        set;
    }

    public int $prop4 = 0 {
        get;
        set { $this->prop4 = $value * 2; }
    }
}

$test = new Test;
try {
    $test->prop = 10;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
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
try {
    var_dump(isset($test->prop2));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$test->prop3 = 1;
try {
    var_dump($test->prop3);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($test->prop3));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $test->prop4 = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop4);
var_dump(isset($test->prop4));

var_dump($test);

?>
--EXPECT--
Cannot recursively write Test::$prop in accessor
Cannot recursively read Test::$prop in accessor
Cannot recursively read Test::$prop in accessor
Cannot recursively read Test::$prop2 in accessor
Cannot recursively read Test::$prop3 in accessor
Cannot recursively read Test::$prop3 in accessor
Cannot recursively write Test::$prop4 in accessor
int(0)
bool(true)
object(Test)#1 (2) {
  ["prop3"]=>
  int(1)
  ["prop4"]=>
  int(0)
}
