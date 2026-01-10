--TEST--
Test caching of hooked property
--FILE--
<?php

class Test {
    public $prop {
        get { echo __METHOD__, "\n"; return $this->prop; }
        set { echo __METHOD__, "\n"; $this->prop = $value; }
    }
}

function doTest(Test $test) {
    $test->prop = null;
    $test->prop;
    $test->prop = 1;
    $test->prop += 1;
    $test->prop = [];
    try {
        $test->prop[] = 1;
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
    isset($test->prop);
    isset($test->prop[0]);
    try {
        unset($test->prop);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

$test = new Test;
$test->dyn = 1;
doTest($test);
echo "\n";
doTest($test);

?>
--EXPECTF--
Deprecated: Creation of dynamic property Test::$dyn is deprecated in %s on line %d
Test::$prop::set
Test::$prop::get
Test::$prop::set
Test::$prop::get
Test::$prop::set
Test::$prop::set
Test::$prop::get
Indirect modification of Test::$prop is not allowed
Test::$prop::get
Test::$prop::get
Cannot unset hooked property Test::$prop

Test::$prop::set
Test::$prop::get
Test::$prop::set
Test::$prop::get
Test::$prop::set
Test::$prop::set
Test::$prop::get
Indirect modification of Test::$prop is not allowed
Test::$prop::get
Test::$prop::get
Cannot unset hooked property Test::$prop
