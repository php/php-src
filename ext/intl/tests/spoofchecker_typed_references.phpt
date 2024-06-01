--TEST--
SpoofChecker with typed references
--EXTENSIONS--
intl
--FILE--
<?php

class Test {
    public string $x;
}

$test = new Test;
$test->x = "";

$checker = new Spoofchecker();
$checker->isSuspicious("", $test->x);
var_dump($test);

$test = new Test;
$test->x = "";

$checker = new Spoofchecker();
$checker->areConfusable("", "", $test->x);
var_dump($test);

?>
--EXPECT--
object(Test)#1 (1) {
  ["x"]=>
  string(1) "0"
}
object(Test)#3 (1) {
  ["x"]=>
  string(1) "1"
}
