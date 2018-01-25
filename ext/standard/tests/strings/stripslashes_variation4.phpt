--TEST--
Test stripslashes() function : usage variations - double dimensional arrays
--FILE--
<?php
/* Prototype  : string stripslashes ( string $str )
 * Description: Returns an un-quoted string
 * Source code: ext/standard/string.c
*/

/*
 * Test stripslashes() with double dimensional arrays
*/

echo "*** Testing stripslashes() : with double dimensional arrays ***\n";

// initialising the string array

$str_array = array(
                    array("", array()),
                    array("", array("")),
                    array("f\\'oo", "b\\'ar", array("fo\\'o", "b\\'ar")),
                    array("f\\'oo", "b\\'ar", array("")),
                    array("f\\'oo", "b\\'ar", array("fo\\'o", "b\\'ar", array(""))),
                    array("f\\'oo", "b\\'ar", array("fo\\'o", "b\\'ar", array("fo\\'o", "b\\'ar")))
                  );
function stripslashes_deep($value)  {
  $value = is_array($value) ? array_map('stripslashes_deep', $value) : stripslashes($value);
  return $value;
}

$count = 1;
// looping to test for all strings in $str_array
foreach( $str_array as $arr )  {
  echo "\n-- Iteration $count --\n";
  var_dump( stripslashes_deep($arr) );
  $count ++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing stripslashes() : with double dimensional arrays ***

-- Iteration 1 --
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(0) {
  }
}

-- Iteration 2 --
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    string(0) ""
  }
}

-- Iteration 3 --
array(3) {
  [0]=>
  string(4) "f'oo"
  [1]=>
  string(4) "b'ar"
  [2]=>
  array(2) {
    [0]=>
    string(4) "fo'o"
    [1]=>
    string(4) "b'ar"
  }
}

-- Iteration 4 --
array(3) {
  [0]=>
  string(4) "f'oo"
  [1]=>
  string(4) "b'ar"
  [2]=>
  array(1) {
    [0]=>
    string(0) ""
  }
}

-- Iteration 5 --
array(3) {
  [0]=>
  string(4) "f'oo"
  [1]=>
  string(4) "b'ar"
  [2]=>
  array(3) {
    [0]=>
    string(4) "fo'o"
    [1]=>
    string(4) "b'ar"
    [2]=>
    array(1) {
      [0]=>
      string(0) ""
    }
  }
}

-- Iteration 6 --
array(3) {
  [0]=>
  string(4) "f'oo"
  [1]=>
  string(4) "b'ar"
  [2]=>
  array(3) {
    [0]=>
    string(4) "fo'o"
    [1]=>
    string(4) "b'ar"
    [2]=>
    array(2) {
      [0]=>
      string(4) "fo'o"
      [1]=>
      string(4) "b'ar"
    }
  }
}
Done
