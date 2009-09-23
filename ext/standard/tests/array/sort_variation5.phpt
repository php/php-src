--TEST--
Test sort() function : usage variations - sort strings 
--FILE--
<?php
/* Prototype  : bool sort ( array &$array [, int $sort_flags] )
 * Description: This function sorts an array. 
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing sort() by providing different string arrays for $array argument with
 * following flag values
 *  flag value as default
 *  SORT_REGULAR - compare items normally
 *  SORT_STRING  - compare items as strings
*/

echo "*** Testing sort() : usage variations ***\n";

$various_arrays = array (
  // group of escape sequences 
  array(null, NULL, "\a", "\cx", "\e", "\f", "\n", "\r", "\t", "\xhh", "\ddd", "\v"),

  // array contains combination of capital/small letters 
  array("lemoN", "Orange", "banana", "apple", "Test", "TTTT", "ttt", "ww", "x", "X", "oraNGe", "BANANA")
);

$flags = array("SORT_REGULAR" => SORT_REGULAR, "SORT_STRING" => SORT_STRING);

$count = 1;
echo "\n-- Testing sort() by supplying various string arrays --\n";

// loop through to test sort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With Default sort flag -\n";
  $temp_array = $array;
  var_dump(sort($temp_array) ); // expecting : bool(true)
  var_dump($temp_array);

  // loop through $flags array and setting all possible flag values
  foreach($flags as $key => $flag){
    echo "- Sort flag = $key -\n";
    $temp_array = $array;
    var_dump(sort($temp_array, $flag) ); // expecting : bool(true)
    var_dump($temp_array);
  }
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing sort() : usage variations ***

-- Testing sort() by supplying various string arrays --

-- Iteration 1 --
- With Default sort flag -
bool(true)
array(12) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  unicode(1) "	"
  [3]=>
  unicode(1) "
"
  [4]=>
  unicode(1) ""
  [5]=>
  unicode(1) ""
  [6]=>
  unicode(1) ""
  [7]=>
  unicode(2) "\a"
  [8]=>
  unicode(3) "\cx"
  [9]=>
  unicode(4) "\ddd"
  [10]=>
  unicode(2) "\e"
  [11]=>
  unicode(4) "\xhh"
}
- Sort flag = SORT_REGULAR -
bool(true)
array(12) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  unicode(1) "	"
  [3]=>
  unicode(1) "
"
  [4]=>
  unicode(1) ""
  [5]=>
  unicode(1) ""
  [6]=>
  unicode(1) ""
  [7]=>
  unicode(2) "\a"
  [8]=>
  unicode(3) "\cx"
  [9]=>
  unicode(4) "\ddd"
  [10]=>
  unicode(2) "\e"
  [11]=>
  unicode(4) "\xhh"
}
- Sort flag = SORT_STRING -
bool(true)
array(12) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  unicode(1) "	"
  [3]=>
  unicode(1) "
"
  [4]=>
  unicode(1) ""
  [5]=>
  unicode(1) ""
  [6]=>
  unicode(1) ""
  [7]=>
  unicode(2) "\a"
  [8]=>
  unicode(3) "\cx"
  [9]=>
  unicode(4) "\ddd"
  [10]=>
  unicode(2) "\e"
  [11]=>
  unicode(4) "\xhh"
}

-- Iteration 2 --
- With Default sort flag -
bool(true)
array(12) {
  [0]=>
  unicode(6) "BANANA"
  [1]=>
  unicode(6) "Orange"
  [2]=>
  unicode(4) "TTTT"
  [3]=>
  unicode(4) "Test"
  [4]=>
  unicode(1) "X"
  [5]=>
  unicode(5) "apple"
  [6]=>
  unicode(6) "banana"
  [7]=>
  unicode(5) "lemoN"
  [8]=>
  unicode(6) "oraNGe"
  [9]=>
  unicode(3) "ttt"
  [10]=>
  unicode(2) "ww"
  [11]=>
  unicode(1) "x"
}
- Sort flag = SORT_REGULAR -
bool(true)
array(12) {
  [0]=>
  unicode(6) "BANANA"
  [1]=>
  unicode(6) "Orange"
  [2]=>
  unicode(4) "TTTT"
  [3]=>
  unicode(4) "Test"
  [4]=>
  unicode(1) "X"
  [5]=>
  unicode(5) "apple"
  [6]=>
  unicode(6) "banana"
  [7]=>
  unicode(5) "lemoN"
  [8]=>
  unicode(6) "oraNGe"
  [9]=>
  unicode(3) "ttt"
  [10]=>
  unicode(2) "ww"
  [11]=>
  unicode(1) "x"
}
- Sort flag = SORT_STRING -
bool(true)
array(12) {
  [0]=>
  unicode(6) "BANANA"
  [1]=>
  unicode(6) "Orange"
  [2]=>
  unicode(4) "TTTT"
  [3]=>
  unicode(4) "Test"
  [4]=>
  unicode(1) "X"
  [5]=>
  unicode(5) "apple"
  [6]=>
  unicode(6) "banana"
  [7]=>
  unicode(5) "lemoN"
  [8]=>
  unicode(6) "oraNGe"
  [9]=>
  unicode(3) "ttt"
  [10]=>
  unicode(2) "ww"
  [11]=>
  unicode(1) "x"
}
Done
