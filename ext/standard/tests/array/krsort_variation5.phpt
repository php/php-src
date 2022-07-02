--TEST--
Test krsort() function : usage variations - sort strings
--FILE--
<?php
/*
 * testing krsort() by providing array of string values for $array argument with
 * following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_STRING  - compare items as strings
*/

echo "*** Testing krsort() : usage variations ***\n";

$various_arrays = array (
  // diff. escape sequence chars with key values
  array ( null =>  null, NULL => NULL, "\a" => "\a", "\cx" => "\cx", "\e" => "\e",
          "\f" => "\f", "\n" =>"\n", "\r" => "\r", "\t" => "\t", "\xhh" => "\xhh",
          "\ddd" => "\ddd", "\v" => "\v"
        ),

  // array containing different strings with key values
  array ( 'Lemon' => "lemoN", 'o' => "Orange", 'B' => "banana", 'Apple' => "apple", 'te' => "Test",
          't' => "TTTT", 'T' => "ttt", 'W' => "ww", 'X' => "x", 'x' => "X", 'O' => "oraNGe",
          'B' => "BANANA"
        )
);

$flags = array("SORT_REGULAR" => SORT_REGULAR, "SORT_STRING" => SORT_STRING);

$count = 1;
echo "\n-- Testing krsort() by supplying various string arrays --\n";

// loop through to test krsort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort flag -\n";
  $temp_array = $array;
  var_dump(krsort($temp_array) ); // expecting : bool(true)
  var_dump($temp_array);

  // loop through $flags array and call krsort() with all possible sort flag values
  foreach($flags as $key => $flag){
    echo "- Sort flag = $key -\n";
    $temp_array = $array;
    var_dump(krsort($temp_array, $flag) ); // expecting : bool(true)
    var_dump($temp_array);
  }
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing krsort() : usage variations ***

-- Testing krsort() by supplying various string arrays --

-- Iteration 1 --
- With default sort flag -
bool(true)
array(11) {
  ["\xhh"]=>
  string(4) "\xhh"
  ["\ddd"]=>
  string(4) "\ddd"
  ["\cx"]=>
  string(3) "\cx"
  ["\a"]=>
  string(2) "\a"
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  ["
"]=>
  string(1) "
"
  ["	"]=>
  string(1) "	"
  [""]=>
  NULL
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  ["\xhh"]=>
  string(4) "\xhh"
  ["\ddd"]=>
  string(4) "\ddd"
  ["\cx"]=>
  string(3) "\cx"
  ["\a"]=>
  string(2) "\a"
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  ["
"]=>
  string(1) "
"
  ["	"]=>
  string(1) "	"
  [""]=>
  NULL
}
- Sort flag = SORT_STRING -
bool(true)
array(11) {
  ["\xhh"]=>
  string(4) "\xhh"
  ["\ddd"]=>
  string(4) "\ddd"
  ["\cx"]=>
  string(3) "\cx"
  ["\a"]=>
  string(2) "\a"
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  ["
"]=>
  string(1) "
"
  ["	"]=>
  string(1) "	"
  [""]=>
  NULL
}

-- Iteration 2 --
- With default sort flag -
bool(true)
array(11) {
  ["x"]=>
  string(1) "X"
  ["te"]=>
  string(4) "Test"
  ["t"]=>
  string(4) "TTTT"
  ["o"]=>
  string(6) "Orange"
  ["X"]=>
  string(1) "x"
  ["W"]=>
  string(2) "ww"
  ["T"]=>
  string(3) "ttt"
  ["O"]=>
  string(6) "oraNGe"
  ["Lemon"]=>
  string(5) "lemoN"
  ["B"]=>
  string(6) "BANANA"
  ["Apple"]=>
  string(5) "apple"
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  ["x"]=>
  string(1) "X"
  ["te"]=>
  string(4) "Test"
  ["t"]=>
  string(4) "TTTT"
  ["o"]=>
  string(6) "Orange"
  ["X"]=>
  string(1) "x"
  ["W"]=>
  string(2) "ww"
  ["T"]=>
  string(3) "ttt"
  ["O"]=>
  string(6) "oraNGe"
  ["Lemon"]=>
  string(5) "lemoN"
  ["B"]=>
  string(6) "BANANA"
  ["Apple"]=>
  string(5) "apple"
}
- Sort flag = SORT_STRING -
bool(true)
array(11) {
  ["x"]=>
  string(1) "X"
  ["te"]=>
  string(4) "Test"
  ["t"]=>
  string(4) "TTTT"
  ["o"]=>
  string(6) "Orange"
  ["X"]=>
  string(1) "x"
  ["W"]=>
  string(2) "ww"
  ["T"]=>
  string(3) "ttt"
  ["O"]=>
  string(6) "oraNGe"
  ["Lemon"]=>
  string(5) "lemoN"
  ["B"]=>
  string(6) "BANANA"
  ["Apple"]=>
  string(5) "apple"
}
Done
