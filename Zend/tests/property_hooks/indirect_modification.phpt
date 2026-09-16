--TEST--
Different kinds of indirect modification with by-val and by-ref getters
--FILE--
<?php


class Test {
    public $byVal {
        set {
            echo __METHOD__, "\n";
            $this->byVal = $value;
        }
    }
}

$test = new Test;

$test->byVal = 0;
$test->byVal++;
++$test->byVal;
$test->byVal += 1;
var_dump($test->byVal);
$test->byVal = [];
try {
    $test->byVal[] = 1;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->byVal);
try {
    $ref =& $test->byVal;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
$ref = 42;
var_dump($test->byVal);

?>
--EXPECT--
Test::$byVal::set
Test::$byVal::set
Test::$byVal::set
Test::$byVal::set
int(3)
Test::$byVal::set
Error: Indirect modification of Test::$byVal is not allowed
array(0) {
}
Error: Indirect modification of Test::$byVal is not allowed
array(0) {
}
