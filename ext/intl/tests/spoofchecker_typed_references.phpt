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

if (version_compare(INTL_ICU_VERSION, '74.0') >= 0) {
    $test = new Test;
    $test->x = "";

    $checker = new Spoofchecker();
    $checker->areBidiConfusable(Spoofchecker::LTR, "", "", $test->x);
    /* Asserted quietly rather than dumped, so that the expected output stays
       the same on ICU < 74, where the method does not exist. */
    if ($test->x !== "1") {
        echo "unexpected value: ";
        var_dump($test->x);
    }
}

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
