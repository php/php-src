--TEST--
Test sort() function : usage variations - SORT_STRICT total ordering, NaN handling, and recursive comparison
--FILE--
<?php
/*
 * Testing sort() with SORT_STRICT flag:
 * 1. Complete type hierarchy ordering
 * 2. NaN handling (sorts after other floats per IEEE 754 totalOrder)
 * 3. Recursive array comparison
 * 4. Recursive object property comparison
 */

echo "*** Testing sort() : SORT_STRICT variations ***\n";

echo "\n-- Complete type hierarchy: null < bool < int < float < string < array < object < resource --\n";
$resource = fopen('php://memory', 'r');
$values = [
    $resource,
    "string",
    42,
    3.14,
    true,
    false,
    null,
    [1, 2, 3],
    (object)['a' => 1],
];
sort($values, SORT_STRICT);
foreach ($values as $v) {
    echo "  " . gettype($v);
    if (is_bool($v)) {
        echo " (" . ($v ? "true" : "false") . ")";
    } elseif (is_scalar($v)) {
        echo " (" . var_export($v, true) . ")";
    }
    echo "\n";
}
fclose($resource);

echo "\n-- NaN sorts after all other floats (IEEE 754 totalOrder) --\n";
$values = [1.0, NAN, -INF, INF, 0.0, NAN, -1.0];
sort($values, SORT_STRICT);
foreach ($values as $v) {
    if (is_nan($v)) {
        echo "  NAN\n";
    } else {
        echo "  " . $v . "\n";
    }
}

echo "\n-- Recursive array comparison --\n";
$values = [
    [1, [2, 3]],
    [1, [2, 2]],
    [1, [2, 4]],
];
sort($values, SORT_STRICT);
var_dump($values);

echo "\n-- Nested arrays with mixed types compared recursively --\n";
$values = [
    ["a" => 1],
    ["a" => "1"],
    ["a" => true],
];
sort($values, SORT_STRICT);
var_dump($values);

echo "\n-- Object property comparison --\n";
class TestClass {
    public $x;
    public $y;
    public function __construct($x, $y) {
        $this->x = $x;
        $this->y = $y;
    }
}
$values = [
    new TestClass(1, 2),
    new TestClass(1, 1),
    new TestClass(1, 3),
];
sort($values, SORT_STRICT);
foreach ($values as $obj) {
    echo "  TestClass(x={$obj->x}, y={$obj->y})\n";
}

echo "\n-- Objects with different property types --\n";
$values = [
    new TestClass(1, "1"),
    new TestClass(1, 1),
    new TestClass(1, true),
];
sort($values, SORT_STRICT);
foreach ($values as $obj) {
    $yType = gettype($obj->y);
    $yVal = var_export($obj->y, true);
    echo "  TestClass(x={$obj->x}, y=$yVal [$yType])\n";
}

echo "\n-- Different classes are compared by properties --\n";
class ClassA {
    public $val = 1;
}
class ClassB {
    public $val = 2;
}
$values = [new ClassB(), new ClassA()];
sort($values, SORT_STRICT);
foreach ($values as $obj) {
    echo "  " . get_class($obj) . "(val={$obj->val})\n";
}

echo "Done";
?>
--EXPECTF--
*** Testing sort() : SORT_STRICT variations ***

-- Complete type hierarchy: null < bool < int < float < string < array < object < resource --
  NULL
  boolean (false)
  boolean (true)
  integer (42)
  double (3.14)
  string ('string')
  array
  object
  resource

-- NaN sorts after all other floats (IEEE 754 totalOrder) --
  -INF
  -1
  0
  1
  INF
  NAN
  NAN

-- Recursive array comparison --
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    array(2) {
      [0]=>
      int(2)
      [1]=>
      int(2)
    }
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    array(2) {
      [0]=>
      int(2)
      [1]=>
      int(3)
    }
  }
  [2]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    array(2) {
      [0]=>
      int(2)
      [1]=>
      int(4)
    }
  }
}

-- Nested arrays with mixed types compared recursively --
array(3) {
  [0]=>
  array(1) {
    ["a"]=>
    bool(true)
  }
  [1]=>
  array(1) {
    ["a"]=>
    int(1)
  }
  [2]=>
  array(1) {
    ["a"]=>
    string(1) "1"
  }
}

-- Object property comparison --
  TestClass(x=1, y=1)
  TestClass(x=1, y=2)
  TestClass(x=1, y=3)

-- Objects with different property types --
  TestClass(x=1, y=true [boolean])
  TestClass(x=1, y=1 [integer])
  TestClass(x=1, y='1' [string])

-- Different classes are compared by properties --
  ClassA(val=1)
  ClassB(val=2)
Done
