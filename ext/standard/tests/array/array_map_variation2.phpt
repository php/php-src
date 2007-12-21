--TEST--
Test array_map() function : usage variations - references
--FILE--
<?php

/* Prototype  : array array_map(mixed callback, array input1 [, array input2 ,...])
 * Description: Applies the callback to the elements in given arrays.
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_map() : references ***\n";
$arr = array("k1" => "v1","k2"=>"v2");
$arr[]=&$arr["k1"];
$arr[]=&$arr;
function cb1 ($a) {var_dump ($a);return array ($a);};
function cb2 (&$a) {var_dump ($a);return array (&$a);};
var_dump( array_map("cb1", $arr));
var_dump( array_map("cb2", $arr,$arr));
var_dump( array_map(null,  $arr));
var_dump( array_map(null, $arr, $arr));

// Break cycles
$arr[0] = null;
$arr[1] = null;

echo "Done";
?>

--EXPECT--
*** Testing array_map() : references ***
string(2) "v1"
string(2) "v2"
string(2) "v1"
array(4) {
  ["k1"]=>
  &string(2) "v1"
  ["k2"]=>
  string(2) "v2"
  [0]=>
  &string(2) "v1"
  [1]=>
  &array(4) {
    ["k1"]=>
    &string(2) "v1"
    ["k2"]=>
    string(2) "v2"
    [0]=>
    &string(2) "v1"
    [1]=>
    &array(4) {
      ["k1"]=>
      &string(2) "v1"
      ["k2"]=>
      string(2) "v2"
      [0]=>
      &string(2) "v1"
      [1]=>
      *RECURSION*
    }
  }
}
array(4) {
  ["k1"]=>
  array(1) {
    [0]=>
    string(2) "v1"
  }
  ["k2"]=>
  array(1) {
    [0]=>
    string(2) "v2"
  }
  [0]=>
  array(1) {
    [0]=>
    string(2) "v1"
  }
  [1]=>
  array(1) {
    [0]=>
    array(4) {
      ["k1"]=>
      &string(2) "v1"
      ["k2"]=>
      string(2) "v2"
      [0]=>
      &string(2) "v1"
      [1]=>
      &array(4) {
        ["k1"]=>
        &string(2) "v1"
        ["k2"]=>
        string(2) "v2"
        [0]=>
        &string(2) "v1"
        [1]=>
        &array(4) {
          ["k1"]=>
          &string(2) "v1"
          ["k2"]=>
          string(2) "v2"
          [0]=>
          &string(2) "v1"
          [1]=>
          *RECURSION*
        }
      }
    }
  }
}
string(2) "v1"
string(2) "v2"
string(2) "v1"
array(4) {
  ["k1"]=>
  &string(2) "v1"
  ["k2"]=>
  string(2) "v2"
  [0]=>
  &string(2) "v1"
  [1]=>
  &array(4) {
    ["k1"]=>
    &string(2) "v1"
    ["k2"]=>
    string(2) "v2"
    [0]=>
    &string(2) "v1"
    [1]=>
    &array(4) {
      ["k1"]=>
      &string(2) "v1"
      ["k2"]=>
      string(2) "v2"
      [0]=>
      &string(2) "v1"
      [1]=>
      *RECURSION*
    }
  }
}
array(4) {
  [0]=>
  array(1) {
    [0]=>
    &string(2) "v1"
  }
  [1]=>
  array(1) {
    [0]=>
    string(2) "v2"
  }
  [2]=>
  array(1) {
    [0]=>
    &string(2) "v1"
  }
  [3]=>
  array(1) {
    [0]=>
    &array(4) {
      ["k1"]=>
      &string(2) "v1"
      ["k2"]=>
      string(2) "v2"
      [0]=>
      &string(2) "v1"
      [1]=>
      &array(4) {
        ["k1"]=>
        &string(2) "v1"
        ["k2"]=>
        string(2) "v2"
        [0]=>
        &string(2) "v1"
        [1]=>
        *RECURSION*
      }
    }
  }
}
array(4) {
  ["k1"]=>
  &string(2) "v1"
  ["k2"]=>
  string(2) "v2"
  [0]=>
  &string(2) "v1"
  [1]=>
  &array(4) {
    ["k1"]=>
    &string(2) "v1"
    ["k2"]=>
    string(2) "v2"
    [0]=>
    &string(2) "v1"
    [1]=>
    &array(4) {
      ["k1"]=>
      &string(2) "v1"
      ["k2"]=>
      string(2) "v2"
      [0]=>
      &string(2) "v1"
      [1]=>
      *RECURSION*
    }
  }
}
array(4) {
  [0]=>
  array(2) {
    [0]=>
    &string(2) "v1"
    [1]=>
    &string(2) "v1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "v2"
    [1]=>
    string(2) "v2"
  }
  [2]=>
  array(2) {
    [0]=>
    &string(2) "v1"
    [1]=>
    &string(2) "v1"
  }
  [3]=>
  array(2) {
    [0]=>
    &array(4) {
      ["k1"]=>
      &string(2) "v1"
      ["k2"]=>
      string(2) "v2"
      [0]=>
      &string(2) "v1"
      [1]=>
      &array(4) {
        ["k1"]=>
        &string(2) "v1"
        ["k2"]=>
        string(2) "v2"
        [0]=>
        &string(2) "v1"
        [1]=>
        *RECURSION*
      }
    }
    [1]=>
    &array(4) {
      ["k1"]=>
      &string(2) "v1"
      ["k2"]=>
      string(2) "v2"
      [0]=>
      &string(2) "v1"
      [1]=>
      &array(4) {
        ["k1"]=>
        &string(2) "v1"
        ["k2"]=>
        string(2) "v2"
        [0]=>
        &string(2) "v1"
        [1]=>
        *RECURSION*
      }
    }
  }
}
Done
