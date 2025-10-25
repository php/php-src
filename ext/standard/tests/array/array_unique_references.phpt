--TEST--
Test array_unique() function : proper handling of references (ZVAL_DEREF)
--FILE--
<?php
echo "*** Testing array_unique() with references (ZVAL_DEREF behavior) ***\n";

// Test 1: Integer references (tests integer-only optimization path)
echo "\n-- Integer references (integer-only path) --\n";
$a = 5;
$b = 5;
$arr = [&$a, &$b, 10];
var_dump(array_unique($arr, SORT_REGULAR));

// Test 2: Reference to same variable
echo "\n-- Reference to same variable --\n";
$x = 42;
$arr = [&$x, &$x, 42];
var_dump(array_unique($arr, SORT_REGULAR));

// Test 3: String references (tests bucket-based path with ZVAL_DEREF)
echo "\n-- String references (bucket path) --\n";
$str1 = "hello";
$str2 = "hello";
$arr = [&$str1, &$str2, "hello"];
var_dump(array_unique($arr, SORT_REGULAR));

// Test 4: Mixed types with references (tests bucket path)
echo "\n-- Mixed types with references --\n";
$int = 5;
$str = "test";
$arr = [&$int, &$str, 5, "test"];
var_dump(array_unique($arr, SORT_REGULAR));

// Test 5: References with SORT_NUMERIC
echo "\n-- References with SORT_NUMERIC --\n";
$n1 = 15;
$n2 = 15;
$arr = [&$n1, 20, &$n2];
var_dump(array_unique($arr, SORT_NUMERIC));

// Test 6: References with SORT_STRING
echo "\n-- References with SORT_STRING --\n";
$s1 = "abc";
$s2 = "abc";
$arr = [&$s1, "def", &$s2];
var_dump(array_unique($arr, SORT_STRING));

// Test 7: Arrays with references (tests complex types path)
echo "\n-- Array references (complex types path) --\n";
$arr1 = [1, 2];
$arr2 = [1, 2];
$arr = [&$arr1, &$arr2, [1, 3]];
var_dump(array_unique($arr, SORT_REGULAR));

// Test 8: Float references
echo "\n-- Float references --\n";
$f1 = 3.14;
$f2 = 3.14;
$arr = [&$f1, &$f2, 2.71];
var_dump(array_unique($arr, SORT_REGULAR));

// Test 9: References preserve keys
echo "\n-- References preserve keys --\n";
$v1 = "test";
$v2 = "test";
$arr = ["a" => &$v1, "b" => &$v2, "c" => "test"];
var_dump(array_unique($arr, SORT_REGULAR));

// Test 10: Type coercion with references in SORT_REGULAR
echo "\n-- Type coercion with references --\n";
$int = 1;
$str = "1";
$arr = [&$int, &$str, 1, "1"];
var_dump(array_unique($arr, SORT_REGULAR));

echo "\nDone\n";
?>
--EXPECT--
*** Testing array_unique() with references (ZVAL_DEREF behavior) ***

-- Integer references (integer-only path) --
array(2) {
  [0]=>
  &int(5)
  [2]=>
  int(10)
}

-- Reference to same variable --
array(1) {
  [0]=>
  &int(42)
}

-- String references (bucket path) --
array(1) {
  [0]=>
  &string(5) "hello"
}

-- Mixed types with references --
array(2) {
  [0]=>
  &int(5)
  [1]=>
  &string(4) "test"
}

-- References with SORT_NUMERIC --
array(2) {
  [0]=>
  &int(15)
  [1]=>
  int(20)
}

-- References with SORT_STRING --
array(2) {
  [0]=>
  &string(3) "abc"
  [1]=>
  string(3) "def"
}

-- Array references (complex types path) --
array(2) {
  [0]=>
  &array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [2]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(3)
  }
}

-- Float references --
array(2) {
  [0]=>
  &float(3.14)
  [2]=>
  float(2.71)
}

-- References preserve keys --
array(1) {
  ["a"]=>
  &string(4) "test"
}

-- Type coercion with references --
array(1) {
  [0]=>
  &int(1)
}

Done
