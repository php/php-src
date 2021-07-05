--TEST--
Test interaction with cache slots
--FILE--
<?php

class Test {
    public readonly string $prop;
    public readonly array $prop2;
    public function setProp(string $prop) {
        $this->prop = $prop;
    }
    public function initAndAppendProp2() {
        $this->prop2 = [];
        $this->prop2[] = 1;
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

?>
--EXPECT--
string(1) "a"
Cannot modify readonly property Test::$prop
string(1) "a"

Cannot modify readonly property Test::$prop2
Cannot modify readonly property Test::$prop2
array(0) {
}
