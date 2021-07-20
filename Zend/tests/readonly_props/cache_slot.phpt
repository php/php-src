--TEST--
Test interaction with cache slots
--FILE--
<?php

class Test {
    public readonly string $prop;
    public readonly array $prop2;
    public readonly object $prop3;
    public function setProp(string $prop) {
        $this->prop = $prop;
    }
    public function initAndAppendProp2() {
        $this->prop2 = [];
        $this->prop2[] = 1;
    }
    public function initProp3() {
        $this->prop3 = new stdClass;
        $this->prop3->foo = 1;
    }
    public function replaceProp3() {
        $ref =& $this->prop3;
        $ref = new stdClass;
    }
}

$test = new Test;
$test->setProp("a");
var_dump($test->prop);
try {
    $test->setProp("b");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop);
echo "\n";

$test = new Test;
try {
    $test->initAndAppendProp2();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->initAndAppendProp2();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop2);
echo "\n";

$test = new Test;
$test->initProp3();
$test->replaceProp3();
var_dump($test->prop3);
$test->replaceProp3();
var_dump($test->prop3);
echo "\n";

// Test variations using closure rebinding, so we have unknown property_info in JIT.
$test = new Test;
(function() { $this->prop2 = []; })->call($test);
$appendProp2 = (function() {
    $this->prop2[] = 1;
})->bindTo($test, Test::class);
try {
    $appendProp2();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $appendProp2();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop2);
echo "\n";

$test = new Test;
$replaceProp3 = (function() {
    $ref =& $this->prop3;
    $ref = new stdClass;
})->bindTo($test, Test::class);
$test->initProp3();
$replaceProp3();
var_dump($test->prop3);
$replaceProp3();
var_dump($test->prop3);

?>
--EXPECT--
string(1) "a"
Cannot modify readonly property Test::$prop
string(1) "a"

Cannot modify readonly property Test::$prop2
Cannot modify readonly property Test::$prop2
array(0) {
}

object(stdClass)#3 (1) {
  ["foo"]=>
  int(1)
}
object(stdClass)#3 (1) {
  ["foo"]=>
  int(1)
}

Cannot modify readonly property Test::$prop2
Cannot modify readonly property Test::$prop2
array(0) {
}

object(stdClass)#5 (1) {
  ["foo"]=>
  int(1)
}
object(stdClass)#5 (1) {
  ["foo"]=>
  int(1)
}
