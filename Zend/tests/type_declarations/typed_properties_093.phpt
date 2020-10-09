--TEST--
Typed property assignment by ref with variable name
--FILE--
<?php

class Test {
    public int $prop;
}

$name = new class {
    public function __toString() {
        return 'prop';
    }
};

$test = new Test;
$ref = "foobar";
try {
    $test->$name =& $ref;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test);

?>
--EXPECT--
Cannot assign string to property Test::$prop of type int
object(Test)#2 (0) {
  ["prop"]=>
  uninitialized(int)
}
