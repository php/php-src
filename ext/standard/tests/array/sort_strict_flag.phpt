--TEST--
array_unique() and sorting functions: SORT_STRICT flag
--FILE--
<?php

function test_values($array) {
    var_dump(array_unique($array, SORT_STRICT));
    $sorted = [] + $array; // Make a copy so we can keep using $array
    var_dump(sort($sorted, SORT_STRICT));
    var_dump($sorted);
    $sorted = array_merge([], $array);
    var_dump(rsort($sorted, SORT_STRICT));
    var_dump($sorted);
    $sorted_multi = array_merge([], $array);
    var_dump(array_multisort($sorted_multi, SORT_DESC, SORT_STRICT));
    var_dump($sorted == $sorted_multi);
}

echo "=== TEST COMPARE VALUES ===\n";

test_values([1, "1", 2, 1, "2", 2.0]);

test_values([null, "hello world", "test", true, false, "abc", null]);

$referenced1 = 1;
test_values([&$referenced1, 1, 0, 2, 1]);

$resource = fopen("php://stdin", "r");

$referenced2 = 2;
$obj = new stdClass;
test_values([true, "a", $resource, &$referenced1, &$referenced1, $obj, null, $resource, 1.5, 1.5, 0, 1, 1.0, false, [], $obj, false, null, true]);
$array = [1, 2];
test_values([$array, true, false, "b", "a", &$referenced2, "a", &$referenced1, new stdClass, $resource, new stdClass, null, 1, null, -5, -5.0, 5.0, 1.0, false, [], false, $array]);

fclose($resource);

echo "=== TEST COMPARE KEYS ===\n";

function test_keys($array) {
    $sorted = [] + $array; // Make a copy so we can keep using $array
    var_dump(ksort($sorted, SORT_STRICT));
    var_dump($sorted);
    $sorted = array_merge([], $array);
    var_dump(krsort($sorted, SORT_STRICT));
    var_dump($sorted);
}

test_keys([5 => "b", 2 => "a", 3 => "c"]);

test_keys(["b" => 0, "c" => 1, "a" => 2]);

test_keys(["b" => 0, 0 => 10, -5 => 11, "c" => 1, "a" => 2, -99 => 1]);

?>
--EXPECT--
=== TEST COMPARE VALUES ===
array(5) {
  [0]=>
  int(1)
  [1]=>
  string(1) "1"
  [2]=>
  int(2)
  [4]=>
  string(1) "2"
  [5]=>
  float(2)
}
bool(true)
array(6) {
  [0]=>
  float(2)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(2)
  [4]=>
  string(1) "1"
  [5]=>
  string(1) "2"
}
bool(true)
array(6) {
  [0]=>
  string(1) "2"
  [1]=>
  string(1) "1"
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
  [5]=>
  float(2)
}
bool(true)
bool(true)
array(6) {
  [0]=>
  NULL
  [1]=>
  string(11) "hello world"
  [2]=>
  string(4) "test"
  [3]=>
  bool(true)
  [4]=>
  bool(false)
  [5]=>
  string(3) "abc"
}
bool(true)
array(7) {
  [0]=>
  bool(false)
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  string(3) "abc"
  [4]=>
  string(11) "hello world"
  [5]=>
  string(4) "test"
  [6]=>
  bool(true)
}
bool(true)
array(7) {
  [0]=>
  bool(true)
  [1]=>
  string(4) "test"
  [2]=>
  string(11) "hello world"
  [3]=>
  string(3) "abc"
  [4]=>
  NULL
  [5]=>
  NULL
  [6]=>
  bool(false)
}
bool(true)
bool(true)
array(4) {
  [0]=>
  &int(1)
  [1]=>
  int(1)
  [2]=>
  int(0)
  [3]=>
  int(2)
}
bool(true)
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(2)
  [4]=>
  &int(1)
}
bool(true)
array(5) {
  [0]=>
  &int(1)
  [1]=>
  int(2)
  [2]=>
  int(1)
  [3]=>
  int(1)
  [4]=>
  int(0)
}
bool(true)
bool(true)
array(12) {
  [0]=>
  bool(true)
  [1]=>
  string(1) "a"
  [2]=>
  resource(5) of type (stream)
  [3]=>
  &int(1)
  [5]=>
  object(stdClass)#1 (0) {
  }
  [6]=>
  NULL
  [8]=>
  float(1.5)
  [10]=>
  int(0)
  [11]=>
  int(1)
  [12]=>
  float(1)
  [13]=>
  bool(false)
  [14]=>
  array(0) {
  }
}
bool(true)
array(19) {
  [0]=>
  array(0) {
  }
  [1]=>
  bool(false)
  [2]=>
  bool(false)
  [3]=>
  float(1)
  [4]=>
  float(1.5)
  [5]=>
  float(1.5)
  [6]=>
  int(0)
  [7]=>
  int(1)
  [8]=>
  NULL
  [9]=>
  NULL
  [10]=>
  object(stdClass)#1 (0) {
  }
  [11]=>
  object(stdClass)#1 (0) {
  }
  [12]=>
  &int(1)
  [13]=>
  &int(1)
  [14]=>
  resource(5) of type (stream)
  [15]=>
  resource(5) of type (stream)
  [16]=>
  string(1) "a"
  [17]=>
  bool(true)
  [18]=>
  bool(true)
}
bool(true)
array(19) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  string(1) "a"
  [3]=>
  resource(5) of type (stream)
  [4]=>
  resource(5) of type (stream)
  [5]=>
  &int(1)
  [6]=>
  &int(1)
  [7]=>
  object(stdClass)#1 (0) {
  }
  [8]=>
  object(stdClass)#1 (0) {
  }
  [9]=>
  NULL
  [10]=>
  NULL
  [11]=>
  int(1)
  [12]=>
  int(0)
  [13]=>
  float(1.5)
  [14]=>
  float(1.5)
  [15]=>
  float(1)
  [16]=>
  bool(false)
  [17]=>
  bool(false)
  [18]=>
  array(0) {
  }
}
bool(true)
bool(true)
array(16) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  bool(true)
  [2]=>
  bool(false)
  [3]=>
  string(1) "b"
  [4]=>
  string(1) "a"
  [5]=>
  &int(2)
  [7]=>
  &int(1)
  [8]=>
  object(stdClass)#2 (0) {
  }
  [9]=>
  resource(5) of type (stream)
  [11]=>
  NULL
  [12]=>
  int(1)
  [14]=>
  int(-5)
  [15]=>
  float(-5)
  [16]=>
  float(5)
  [17]=>
  float(1)
  [19]=>
  array(0) {
  }
}
bool(true)
array(22) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(2) {
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
    int(2)
  }
  [3]=>
  bool(false)
  [4]=>
  bool(false)
  [5]=>
  bool(false)
  [6]=>
  float(-5)
  [7]=>
  float(1)
  [8]=>
  float(5)
  [9]=>
  int(-5)
  [10]=>
  int(1)
  [11]=>
  NULL
  [12]=>
  NULL
  [13]=>
  object(stdClass)#2 (0) {
  }
  [14]=>
  object(stdClass)#3 (0) {
  }
  [15]=>
  &int(1)
  [16]=>
  &int(2)
  [17]=>
  resource(5) of type (stream)
  [18]=>
  string(1) "a"
  [19]=>
  string(1) "a"
  [20]=>
  string(1) "b"
  [21]=>
  bool(true)
}
bool(true)
array(22) {
  [0]=>
  bool(true)
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "a"
  [3]=>
  string(1) "a"
  [4]=>
  resource(5) of type (stream)
  [5]=>
  &int(2)
  [6]=>
  &int(1)
  [7]=>
  object(stdClass)#2 (0) {
  }
  [8]=>
  object(stdClass)#3 (0) {
  }
  [9]=>
  NULL
  [10]=>
  NULL
  [11]=>
  int(1)
  [12]=>
  int(-5)
  [13]=>
  float(5)
  [14]=>
  float(1)
  [15]=>
  float(-5)
  [16]=>
  bool(false)
  [17]=>
  bool(false)
  [18]=>
  bool(false)
  [19]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [20]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [21]=>
  array(0) {
  }
}
bool(true)
bool(true)
=== TEST COMPARE KEYS ===
bool(true)
array(3) {
  [2]=>
  string(1) "a"
  [3]=>
  string(1) "c"
  [5]=>
  string(1) "b"
}
bool(true)
array(3) {
  [2]=>
  string(1) "c"
  [1]=>
  string(1) "a"
  [0]=>
  string(1) "b"
}
bool(true)
array(3) {
  ["a"]=>
  int(2)
  ["b"]=>
  int(0)
  ["c"]=>
  int(1)
}
bool(true)
array(3) {
  ["c"]=>
  int(1)
  ["b"]=>
  int(0)
  ["a"]=>
  int(2)
}
bool(true)
array(6) {
  [-99]=>
  int(1)
  [-5]=>
  int(11)
  [0]=>
  int(10)
  ["a"]=>
  int(2)
  ["b"]=>
  int(0)
  ["c"]=>
  int(1)
}
bool(true)
array(6) {
  ["c"]=>
  int(1)
  ["b"]=>
  int(0)
  ["a"]=>
  int(2)
  [2]=>
  int(1)
  [1]=>
  int(11)
  [0]=>
  int(10)
}
