--TEST--
Test array_merge and array_walk
--INI--
precision=14
--FILE--
<?php
require(dirname(__FILE__) . '/data.inc');
/*
** Create sample arrays
** Test alpha, numeric (decimal, hex, octal) and special data
**
**
*/

/* Helper function to build testing arrays */
function make_nested_array ($depth, $breadth, $function = NULL, $args = array ()) {
    for ($x = 0; $x < $breadth; ++$x) {
        if (NULL === $function) {
            $array = array (0);
        } else {
            $array = array (call_user_func_array ($function, $args));
        }
        for ($y = 1; $y < $depth; ++$y) {
            $array[0] = array ($array[0]);
        }
        $temp[$x] = $array;
    }
    return $temp;
}

/* Nested array */
$data2 = make_nested_array (3, 3);
$data = array_merge($data, $data2);

var_dump ($data);

function echo_kv ($value, $key) {
    var_dump ($key);
    var_dump ($value);
}

echo " -- Testing array_walk() -- \n";
array_walk ($data, 'echo_kv');

?>
--EXPECT--
array(11) {
  [0]=>
  string(3) "PHP"
  [1]=>
  string(27) "PHP: Hypertext Preprocessor"
  [2]=>
  string(4) "Test"
  ["test"]=>
  int(27)
  [3]=>
  string(4) "test"
  [4]=>
  array(2) {
    [0]=>
    string(6) "banana"
    [1]=>
    string(6) "orange"
  }
  [5]=>
  string(6) "monkey"
  [6]=>
  float(-0.33333333333333)
  [7]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        int(0)
      }
    }
  }
  [8]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        int(0)
      }
    }
  }
  [9]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        int(0)
      }
    }
  }
}
 -- Testing array_walk() -- 
int(0)
string(3) "PHP"
int(1)
string(27) "PHP: Hypertext Preprocessor"
int(2)
string(4) "Test"
string(4) "test"
int(27)
int(3)
string(4) "test"
int(4)
array(2) {
  [0]=>
  string(6) "banana"
  [1]=>
  string(6) "orange"
}
int(5)
string(6) "monkey"
int(6)
float(-0.33333333333333)
int(7)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      int(0)
    }
  }
}
int(8)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      int(0)
    }
  }
}
int(9)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      int(0)
    }
  }
}
