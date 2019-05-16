--TEST--
Test asort() function : usage variations - sort strings
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
  die("skip Output tested contains chars that are not shown the same on windows concole (ESC and co)");
}
--FILE--
<?php
/* Prototype  : bool asort ( array &$array [, int $asort_flags] )
 * Description: Sort an array and maintain index association
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing asort() by providing different string arrays for $array argument with following flag values
 *  flag value as defualt
 *  SORT_REGULAR - compare items normally
 *  SORT_STRING  - compare items as strings
*/

echo "*** Testing asort() : usage variations ***\n";

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
echo "\n-- Testing asort() by supplying various string arrays --\n";

// loop through to test asort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort_flag -\n";
  $temp_array = $array;
  var_dump(asort($temp_array) ); // expecting : bool(true)
  var_dump($temp_array);

  // loop through $flags array and setting all possible flag values
  foreach($flags as $key => $flag){
    echo "- Sort_flag = $key -\n";
    $temp_array = $array;
    var_dump(asort($temp_array, $flag) ); // expecting : bool(true)
    var_dump($temp_array);
  }
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing asort() : usage variations ***

-- Testing asort() by supplying various string arrays --

-- Iteration 1 --
- With default sort_flag -
bool(true)
array(12) {
  ["null"]=>
  NULL
  ["NULL"]=>
  NULL
  ["	"]=>
  string(1) "	"
  ["
"]=>
  string(1) "
"
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  ["\a"]=>
  string(2) "\a"
  ["\cx"]=>
  string(3) "\cx"
  ["\ddd"]=>
  string(4) "\ddd"
  ["\xhh"]=>
  string(4) "\xhh"
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(12) {
  ["null"]=>
  NULL
  ["NULL"]=>
  NULL
  ["	"]=>
  string(1) "	"
  ["
"]=>
  string(1) "
"
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  ["\a"]=>
  string(2) "\a"
  ["\cx"]=>
  string(3) "\cx"
  ["\ddd"]=>
  string(4) "\ddd"
  ["\xhh"]=>
  string(4) "\xhh"
}
- Sort_flag = SORT_STRING -
bool(true)
array(12) {
  ["null"]=>
  NULL
  ["NULL"]=>
  NULL
  ["	"]=>
  string(1) "	"
  ["
"]=>
  string(1) "
"
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  [""]=>
  string(1) ""
  ["\a"]=>
  string(2) "\a"
  ["\cx"]=>
  string(3) "\cx"
  ["\ddd"]=>
  string(4) "\ddd"
  ["\xhh"]=>
  string(4) "\xhh"
}

-- Iteration 2 --
- With default sort_flag -
bool(true)
array(12) {
  ["B"]=>
  string(6) "BANANA"
  ["O"]=>
  string(6) "Orange"
  ["T"]=>
  string(4) "TTTT"
  ["Te"]=>
  string(4) "Test"
  ["X"]=>
  string(1) "X"
  ["a"]=>
  string(5) "apple"
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemoN"
  ["o"]=>
  string(6) "oraNGe"
  ["t"]=>
  string(3) "ttt"
  ["w"]=>
  string(2) "ww"
  ["x"]=>
  string(1) "x"
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(12) {
  ["B"]=>
  string(6) "BANANA"
  ["O"]=>
  string(6) "Orange"
  ["T"]=>
  string(4) "TTTT"
  ["Te"]=>
  string(4) "Test"
  ["X"]=>
  string(1) "X"
  ["a"]=>
  string(5) "apple"
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemoN"
  ["o"]=>
  string(6) "oraNGe"
  ["t"]=>
  string(3) "ttt"
  ["w"]=>
  string(2) "ww"
  ["x"]=>
  string(1) "x"
}
- Sort_flag = SORT_STRING -
bool(true)
array(12) {
  ["B"]=>
  string(6) "BANANA"
  ["O"]=>
  string(6) "Orange"
  ["T"]=>
  string(4) "TTTT"
  ["Te"]=>
  string(4) "Test"
  ["X"]=>
  string(1) "X"
  ["a"]=>
  string(5) "apple"
  ["b"]=>
  string(6) "banana"
  ["l"]=>
  string(5) "lemoN"
  ["o"]=>
  string(6) "oraNGe"
  ["t"]=>
  string(3) "ttt"
  ["w"]=>
  string(2) "ww"
  ["x"]=>
  string(1) "x"
}
Done
