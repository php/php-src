--TEST--
Test array_merge_deep() function : basic functionality - associative arrays with non array values
--FILE--
<?php
echo "*** Testing array_merge_deep() : associative arrays with non array values ***\n";

// Initialise the arrays
$arr1 = array("a" => 1, "b" => ["one"], "c" => [3]);
$arr2 = array("a" => [2], "b" => "two", "c" => ["three"]);
$arr3 = array("a" => [2.2], "b" => ["three" => "three"], "c" => [3.3]);
$arr4 = array("a" => "22", "b" => ["three" => "four"], "c" => ["three" => ["3"]]);
$arr5 = array("c" => ["three" => ["three"]]);
$arr6 = array("c" => "one");

var_dump( array_merge_deep($arr1,$arr2) );
var_dump( array_merge_deep($arr1,$arr2,$arr3) );
var_dump( array_merge_deep($arr1,$arr2,$arr3,$arr4) );
var_dump( array_merge_deep($arr1,$arr2,$arr3,$arr4,$arr5) );
var_dump( array_merge_deep($arr1,$arr2,$arr3,$arr4,$arr5,$arr6) );

echo "Done";
?>
--EXPECT--
*** Testing array_merge_deep() : associative arrays with non array values ***
array(3) {
  ["a"]=>
  array(1) {
    [0]=>
    int(2)
  }
  ["b"]=>
  string(3) "two"
  ["c"]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    string(5) "three"
  }
}
array(3) {
  ["a"]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    float(2.2)
  }
  ["b"]=>
  array(1) {
    ["three"]=>
    string(5) "three"
  }
  ["c"]=>
  array(3) {
    [0]=>
    int(3)
    [1]=>
    string(5) "three"
    [2]=>
    float(3.3)
  }
}
array(3) {
  ["a"]=>
  string(2) "22"
  ["b"]=>
  array(1) {
    ["three"]=>
    string(4) "four"
  }
  ["c"]=>
  array(4) {
    [0]=>
    int(3)
    [1]=>
    string(5) "three"
    [2]=>
    float(3.3)
    ["three"]=>
    array(1) {
      [0]=>
      string(1) "3"
    }
  }
}
array(3) {
  ["a"]=>
  string(2) "22"
  ["b"]=>
  array(1) {
    ["three"]=>
    string(4) "four"
  }
  ["c"]=>
  array(4) {
    [0]=>
    int(3)
    [1]=>
    string(5) "three"
    [2]=>
    float(3.3)
    ["three"]=>
    array(2) {
      [0]=>
      string(1) "3"
      [1]=>
      string(5) "three"
    }
  }
}
array(3) {
  ["a"]=>
  string(2) "22"
  ["b"]=>
  array(1) {
    ["three"]=>
    string(4) "four"
  }
  ["c"]=>
  string(3) "one"
}
Done
