--TEST--
Test krsort() function : usage variations - sort strings  
--FILE--
<?php
/* Prototype  : bool krsort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key in reverse order, maintaining key to data correlation
 * Source code: ext/standard/array.c
*/

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
  [u"\xhh"]=>
  unicode(4) "\xhh"
  [u"\e"]=>
  unicode(2) "\e"
  [u"\ddd"]=>
  unicode(4) "\ddd"
  [u"\cx"]=>
  unicode(3) "\cx"
  [u"\a"]=>
  unicode(2) "\a"
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u"
"]=>
  unicode(1) "
"
  [u"	"]=>
  unicode(1) "	"
  [u""]=>
  NULL
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  [u"\xhh"]=>
  unicode(4) "\xhh"
  [u"\e"]=>
  unicode(2) "\e"
  [u"\ddd"]=>
  unicode(4) "\ddd"
  [u"\cx"]=>
  unicode(3) "\cx"
  [u"\a"]=>
  unicode(2) "\a"
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u"
"]=>
  unicode(1) "
"
  [u"	"]=>
  unicode(1) "	"
  [u""]=>
  NULL
}
- Sort flag = SORT_STRING -
bool(true)
array(11) {
  [u"\xhh"]=>
  unicode(4) "\xhh"
  [u"\e"]=>
  unicode(2) "\e"
  [u"\ddd"]=>
  unicode(4) "\ddd"
  [u"\cx"]=>
  unicode(3) "\cx"
  [u"\a"]=>
  unicode(2) "\a"
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u"
"]=>
  unicode(1) "
"
  [u"	"]=>
  unicode(1) "	"
  [u""]=>
  NULL
}

-- Iteration 2 --
- With default sort flag -
bool(true)
array(11) {
  [u"x"]=>
  unicode(1) "X"
  [u"te"]=>
  unicode(4) "Test"
  [u"t"]=>
  unicode(4) "TTTT"
  [u"o"]=>
  unicode(6) "Orange"
  [u"X"]=>
  unicode(1) "x"
  [u"W"]=>
  unicode(2) "ww"
  [u"T"]=>
  unicode(3) "ttt"
  [u"O"]=>
  unicode(6) "oraNGe"
  [u"Lemon"]=>
  unicode(5) "lemoN"
  [u"B"]=>
  unicode(6) "BANANA"
  [u"Apple"]=>
  unicode(5) "apple"
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  [u"x"]=>
  unicode(1) "X"
  [u"te"]=>
  unicode(4) "Test"
  [u"t"]=>
  unicode(4) "TTTT"
  [u"o"]=>
  unicode(6) "Orange"
  [u"X"]=>
  unicode(1) "x"
  [u"W"]=>
  unicode(2) "ww"
  [u"T"]=>
  unicode(3) "ttt"
  [u"O"]=>
  unicode(6) "oraNGe"
  [u"Lemon"]=>
  unicode(5) "lemoN"
  [u"B"]=>
  unicode(6) "BANANA"
  [u"Apple"]=>
  unicode(5) "apple"
}
- Sort flag = SORT_STRING -
bool(true)
array(11) {
  [u"x"]=>
  unicode(1) "X"
  [u"te"]=>
  unicode(4) "Test"
  [u"t"]=>
  unicode(4) "TTTT"
  [u"o"]=>
  unicode(6) "Orange"
  [u"X"]=>
  unicode(1) "x"
  [u"W"]=>
  unicode(2) "ww"
  [u"T"]=>
  unicode(3) "ttt"
  [u"O"]=>
  unicode(6) "oraNGe"
  [u"Lemon"]=>
  unicode(5) "lemoN"
  [u"B"]=>
  unicode(6) "BANANA"
  [u"Apple"]=>
  unicode(5) "apple"
}
Done
