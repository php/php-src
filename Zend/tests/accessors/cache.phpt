--TEST--
Test caching of accessor property kind
--FILE--
<?php

class Test {
    private $_prop;
    public $prop {
        &get { echo __METHOD__, "\n"; return $this->_prop; }
        set { echo __METHOD__, "\n"; $this->_prop = $value; }
    }
}

function doTest(Test $test) {
    $test->prop;
    $test->prop = 1;
    $test->prop += 1;
    $test->prop = [];
    $test->prop[] = 1;
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
--EXPECT--
Test::$prop::get
Test::$prop::set
Test::$prop::get
Test::$prop::set
Test::$prop::set
Test::$prop::get
Test::$prop::get
Test::$prop::get
Cannot unset accessor property Test::$prop

Test::$prop::get
Test::$prop::set
Test::$prop::get
Test::$prop::set
Test::$prop::set
Test::$prop::get
Test::$prop::get
Test::$prop::get
Cannot unset accessor property Test::$prop
