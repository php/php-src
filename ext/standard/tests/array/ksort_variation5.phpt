--TEST--
Test ksort() function : usage variations - sort strings  
--FILE--
<?php
/* Prototype  : bool ksort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key, maintaining key to data correlation
 * Source code: ext/standard/array.c
*/

/*
 * testing ksort() by providing array of string values for $array argument with
 * following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_STRING  - compare items as strings
*/

echo "*** Testing ksort() : usage variations ***\n";

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
echo "\n-- Testing ksort() by supplying various string arrays --\n";

// loop through to test ksort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort flag -\n";
  $temp_array = $array;
  var_dump(ksort($temp_array) ); // expecting : bool(true)
  var_dump($temp_array);

  // loop through $flags array and call ksort() with all possible sort flag values
  foreach($flags as $key => $flag){
    echo "- Sort flag = $key -\n";
    $temp_array = $array;
    var_dump(ksort($temp_array, $flag) ); // expecting : bool(true)
    var_dump($temp_array);
  }
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing ksort() : usage variations ***

-- Testing ksort() by supplying various string arrays --

-- Iteration 1 --
- With default sort flag -
bool(true)
array(11) {
  [u""]=>
  NULL
  [u"	"]=>
  unicode(1) "	"
  [u"
"]=>
  unicode(1) "
"
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u"\a"]=>
  unicode(2) "\a"
  [u"\cx"]=>
  unicode(3) "\cx"
  [u"\ddd"]=>
  unicode(4) "\ddd"
  [u"\e"]=>
  unicode(2) "\e"
  [u"\xhh"]=>
  unicode(4) "\xhh"
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  [u""]=>
  NULL
  [u"	"]=>
  unicode(1) "	"
  [u"
"]=>
  unicode(1) "
"
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u"\a"]=>
  unicode(2) "\a"
  [u"\cx"]=>
  unicode(3) "\cx"
  [u"\ddd"]=>
  unicode(4) "\ddd"
  [u"\e"]=>
  unicode(2) "\e"
  [u"\xhh"]=>
  unicode(4) "\xhh"
}
- Sort flag = SORT_STRING -
bool(true)
array(11) {
  [u""]=>
  NULL
  [u"	"]=>
  unicode(1) "	"
  [u"
"]=>
  unicode(1) "
"
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u""]=>
  unicode(1) ""
  [u"\a"]=>
  unicode(2) "\a"
  [u"\cx"]=>
  unicode(3) "\cx"
  [u"\ddd"]=>
  unicode(4) "\ddd"
  [u"\e"]=>
  unicode(2) "\e"
  [u"\xhh"]=>
  unicode(4) "\xhh"
}

-- Iteration 2 --
- With default sort flag -
bool(true)
array(11) {
  [u"Apple"]=>
  unicode(5) "apple"
  [u"B"]=>
  unicode(6) "BANANA"
  [u"Lemon"]=>
  unicode(5) "lemoN"
  [u"O"]=>
  unicode(6) "oraNGe"
  [u"T"]=>
  unicode(3) "ttt"
  [u"W"]=>
  unicode(2) "ww"
  [u"X"]=>
  unicode(1) "x"
  [u"o"]=>
  unicode(6) "Orange"
  [u"t"]=>
  unicode(4) "TTTT"
  [u"te"]=>
  unicode(4) "Test"
  [u"x"]=>
  unicode(1) "X"
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  [u"Apple"]=>
  unicode(5) "apple"
  [u"B"]=>
  unicode(6) "BANANA"
  [u"Lemon"]=>
  unicode(5) "lemoN"
  [u"O"]=>
  unicode(6) "oraNGe"
  [u"T"]=>
  unicode(3) "ttt"
  [u"W"]=>
  unicode(2) "ww"
  [u"X"]=>
  unicode(1) "x"
  [u"o"]=>
  unicode(6) "Orange"
  [u"t"]=>
  unicode(4) "TTTT"
  [u"te"]=>
  unicode(4) "Test"
  [u"x"]=>
  unicode(1) "X"
}
- Sort flag = SORT_STRING -
bool(true)
array(11) {
  [u"Apple"]=>
  unicode(5) "apple"
  [u"B"]=>
  unicode(6) "BANANA"
  [u"Lemon"]=>
  unicode(5) "lemoN"
  [u"O"]=>
  unicode(6) "oraNGe"
  [u"T"]=>
  unicode(3) "ttt"
  [u"W"]=>
  unicode(2) "ww"
  [u"X"]=>
  unicode(1) "x"
  [u"o"]=>
  unicode(6) "Orange"
  [u"t"]=>
  unicode(4) "TTTT"
  [u"te"]=>
  unicode(4) "Test"
  [u"x"]=>
  unicode(1) "X"
}
Done
