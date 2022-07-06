--TEST--
basic array_chunk test
--FILE--
<?php
$input_array = array('a', 'b', 'c', 'd', 'e');

try {
    var_dump(array_chunk($input_array, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_chunk($input_array, 0, true));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(array_chunk($input_array, 1));
var_dump(array_chunk($input_array, 1, true));
var_dump(array_chunk($input_array, 2));
var_dump(array_chunk($input_array, 2, true));
var_dump(array_chunk($input_array, 10));
var_dump(array_chunk($input_array, 10, true));
?>
--EXPECT--
array_chunk(): Argument #2 ($length) must be greater than 0
array_chunk(): Argument #2 ($length) must be greater than 0
array(5) {
  [0]=>
  array(1) {
    [0]=>
    string(1) "a"
  }
  [1]=>
  array(1) {
    [0]=>
    string(1) "b"
  }
  [2]=>
  array(1) {
    [0]=>
    string(1) "c"
  }
  [3]=>
  array(1) {
    [0]=>
    string(1) "d"
  }
  [4]=>
  array(1) {
    [0]=>
    string(1) "e"
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    string(1) "a"
  }
  [1]=>
  array(1) {
    [1]=>
    string(1) "b"
  }
  [2]=>
  array(1) {
    [2]=>
    string(1) "c"
  }
  [3]=>
  array(1) {
    [3]=>
    string(1) "d"
  }
  [4]=>
  array(1) {
    [4]=>
    string(1) "e"
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    string(1) "d"
  }
  [2]=>
  array(1) {
    [0]=>
    string(1) "e"
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
  [1]=>
  array(2) {
    [2]=>
    string(1) "c"
    [3]=>
    string(1) "d"
  }
  [2]=>
  array(1) {
    [4]=>
    string(1) "e"
  }
}
array(1) {
  [0]=>
  array(5) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
    [3]=>
    string(1) "d"
    [4]=>
    string(1) "e"
  }
}
array(1) {
  [0]=>
  array(5) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
    [3]=>
    string(1) "d"
    [4]=>
    string(1) "e"
  }
}
