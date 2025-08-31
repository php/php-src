--TEST--
Not-modifying a readonly property holding an object
--FILE--
<?php

class Test {
    public readonly object $prop;

    public function __construct(object $prop) {
        $this->prop = $prop;
    }
}

$test = new Test(new stdClass);
$test->prop->foo = 1;
$test->prop->foo += 1;
$test->prop->foo++;
try {
    $test->prop += 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop++;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    --$test->prop;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop);

// Unfortunately this is allowed, but does not modify $test->prop.
$ref =& $test->prop;
$ref = new stdClass;
var_dump($test->prop);

$test = new Test(new ArrayObject());
$test->prop[] = [];
$test->prop[0][] = 1;
var_dump($test->prop);

?>
--EXPECT--
Unsupported operand types: stdClass + int
Cannot modify readonly property Test::$prop
Cannot modify readonly property Test::$prop
object(stdClass)#2 (1) {
  ["foo"]=>
  int(3)
}
object(stdClass)#2 (1) {
  ["foo"]=>
  int(3)
}
object(ArrayObject)#7 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
}
