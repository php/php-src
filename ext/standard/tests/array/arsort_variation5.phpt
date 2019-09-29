--TEST--
Test arsort() function : usage variations - sort strings
--FILE--
<?php
/* Prototype  : bool arsort ( array &$array [, int $asort_flags] )
 * Description: Sort an array and maintain index association
                Elements will be arranged from highest to lowest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing arsort() by providing different string arrays for $array argument with following flag values
 *  flag value as defualt
 *  SORT_REGULAR - compare items normally
 *  SORT_STRING  - compare items as strings
*/

echo "*** Testing arsort() : usage variations ***\n";

$various_arrays = array (
  // group of escape sequences
  array ("null"=>  null, "NULL" => NULL, "\a" => "\a", "\cx" => "\cx", "\e" => "\e",
        "\f" => "\f", "\n" =>"\n", "\r" => "\r", "\t" => "\t", "\xhh" => "\xhh",
        "\ddd" => "\ddd", "\v" => "\v"
        ),

  // array contains combination of capital/small letters
  array ('l' => "lemoN", 'O' => "Orange", 'b' => "banana", 'a' => "apple", 'Te' => "Test",
        'T' => "TTTT", 't' => "ttt", 'w' => "ww", 'x' => "x", 'X' => "X", 'o' => "oraNGe",
        'B' => "BANANA"
        )
);

$flags = array("SORT_REGULAR" => SORT_REGULAR, "SORT_STRING" => SORT_STRING);

$count = 1;
echo "\n-- Testing arsort() by supplying various string arrays --\n";

// loop through to test arsort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort_flag -\n";
  $temp_array = $array;
  var_dump(arsort($temp_array) ); // expecting : bool(true)
  var_dump($temp_array);

  // loop through $flags array and setting all possible flag values
  foreach($flags as $key => $flag){
    echo "- Sort_flag = $key -\n";
    $temp_array = $array;
    var_dump(arsort($temp_array, $flag) ); // expecting : bool(true)
    var_dump($temp_array);
  }
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing arsort() : usage variations ***

-- Testing arsort() by supplying various string arrays --

-- Iteration 1 --
- With default sort_flag -
bool(true)
array(12) {
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
  ["null"]=>
  NULL
  ["NULL"]=>
  NULL
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(12) {
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
  ["null"]=>
  NULL
  ["NULL"]=>
  NULL
}
- Sort_flag = SORT_STRING -
bool(true)
array(12) {
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
  ["null"]=>
  NULL
  ["NULL"]=>
  NULL
}

-- Iteration 2 --
- With default sort_flag -
bool(true)
array(12) {
  ["x"]=>
  string(1) "x"
  ["w"]=>
  string(2) "ww"
  ["t"]=>
  string(3) "ttt"
  ["o"]=>
  string(6) "oraNGe"
  ["l"]=>
  string(5) "lemoN"
  ["b"]=>
  string(6) "banana"
  ["a"]=>
  string(5) "apple"
  ["X"]=>
  string(1) "X"
  ["Te"]=>
  string(4) "Test"
  ["T"]=>
  string(4) "TTTT"
  ["O"]=>
  string(6) "Orange"
  ["B"]=>
  string(6) "BANANA"
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(12) {
  ["x"]=>
  string(1) "x"
  ["w"]=>
  string(2) "ww"
  ["t"]=>
  string(3) "ttt"
  ["o"]=>
  string(6) "oraNGe"
  ["l"]=>
  string(5) "lemoN"
  ["b"]=>
  string(6) "banana"
  ["a"]=>
  string(5) "apple"
  ["X"]=>
  string(1) "X"
  ["Te"]=>
  string(4) "Test"
  ["T"]=>
  string(4) "TTTT"
  ["O"]=>
  string(6) "Orange"
  ["B"]=>
  string(6) "BANANA"
}
- Sort_flag = SORT_STRING -
bool(true)
array(12) {
  ["x"]=>
  string(1) "x"
  ["w"]=>
  string(2) "ww"
  ["t"]=>
  string(3) "ttt"
  ["o"]=>
  string(6) "oraNGe"
  ["l"]=>
  string(5) "lemoN"
  ["b"]=>
  string(6) "banana"
  ["a"]=>
  string(5) "apple"
  ["X"]=>
  string(1) "X"
  ["Te"]=>
  string(4) "Test"
  ["T"]=>
  string(4) "TTTT"
  ["O"]=>
  string(6) "Orange"
  ["B"]=>
  string(6) "BANANA"
}
Done
