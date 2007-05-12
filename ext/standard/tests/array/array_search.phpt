--TEST--
Test array_search() and in_array() functions (including bug 13567)
--FILE--
<?php
/* Prototype: bool in_array ( mixed $needle, array $haystack [, bool $strict] );
   Description: Checks if a value exists in an array
                Searches haystack for needle and returns TRUE  if found in array, FALSE otherwise.
                If the third parameter strict is set to TRUE then the in_array() function 
                will also check the types of the needle in the haystack. 
*/

$arr1 = array('a','b','c');
$arr2 = array();
$arr3 = array('c','key'=>'d');
$arr4 = array("a\0b"=>'e','key'=>'d', 'f');

$tests = <<<TESTS
FALSE === in_array(123, \$arr2)
FALSE === in_array(123, \$arr1)
FALSE === array_search(123, \$arr1)
TRUE  === in_array('a', \$arr1)
0     === array_search('a', \$arr1)
"a\\0b"=== array_search('e', \$arr4)
'key'=== array_search('d', \$arr4)
TESTS;

include(dirname(__FILE__) . '/../../../../tests/quicktester.inc');

/* checking for STRICT option in in_array() */
echo "\n*** Testing STRICT option of in_array() on arrays ***\n";
$arrays = array (
  array(0),
  array("a" => "A", 2 => "B", "C" => 3, 4 => 4, "one" => 1, "" => NULL, "b", "ab", "abcd"),
  array(4, array(1, 2 => 3), "one" => 1, "5" => 5 ),
  array(-1, -2, -3, -4, -2.989888, "-0.005" => "neg0.005", 2.0 => "float2", "-.9" => -.9),
  array(TRUE, FALSE),
  array("", array()),
  array("abcd\x00abcd\x00abcd"),
  array("abcd\tabcd\nabcd\rabcd\0abcdefghij") 
);

$array_compare = array (
  4,
  "4",
  4.00,
  "b",
  "5",
  -2,
  -2.0,
  -2.98989,
  "-.9",
  "True",
  "",
  array(),
  NULL,
  "ab",
  "abcd",
  0.0,
  -0,
  "abcd\x00abcd\x00abcd"
);
/* loop to check if elements in $array_compare exist in $arrays
   using in_array() */
$counter = 1;
foreach($arrays as $array) {
  foreach($array_compare as $compare) {
    echo "-- Iteration $counter --\n";
    //strict option OFF
    var_dump(in_array($compare,$array));  
    //strict option ON
    var_dump(in_array($compare,$array,TRUE));  
    //strict option OFF
    var_dump(in_array($compare,$array,FALSE));  
    $counter++;
 }
}
		
/* checking loose and strict TYPE comparisons in in_array() */
echo "\n*** Testing loose and strict TYPE comparison of in_array() ***\n";
$misc_array = array (
  'a',
  'key' =>'d',
  3,
  ".001" =>-67, 
  "-.051" =>"k",
  0.091 =>"-.08",
  "e" =>"5", 
  "y" =>NULL,
  NULL =>"",
  0,
  TRUE,
  FALSE,
  -27.39999999999,
  " ",
  "abcd\x00abcd\x00\abcd\x00abcdefghij",
  "abcd\nabcd\tabcd\rabcd\0abcd"
);
$array_type = array(TRUE, FALSE, 1, 0, -1, "1", "0", "-1", NULL, array(), "PHP", "");
/* loop to do loose and strict type check of elements in
   $array_type on elements in $misc_array using in_array();
   checking PHP type comparison tables
*/
$counter = 1;
foreach($array_type as $type) {
  echo "-- Iteration $counter --\n";
  //loose type checking
  var_dump( in_array($type,$misc_array ) );  
  //strict type checking
  var_dump( in_array($type,$misc_array,true) );  
  //loose type checking
  var_dump( in_array($type,$misc_array,false) );  
  $counter++;
}

/* checking for sub-arrays with in_array() */
echo "\n*** Testing sub-arrays with in_array() ***\n";
$sub_array = array (
  "one", 
  array(1, 2 => "two", "three" => 3),
  4 => "four",
  "five" => 5,
  array('', 'i')
);
var_dump( in_array("four", $sub_array) );
//checking for element in a sub-array
var_dump( in_array(3, $sub_array[1]) ); 
var_dump( in_array(array('','i'), $sub_array) );

/* checking for objects in in_array() */
echo "\n*** Testing objects with in_array() ***\n";
class in_array_check {
  public $array_var = array(1=>"one", "two"=>2, 3=>3);
  public function foo() {
    echo "Public function\n";
  }
}

$in_array_obj = new in_array_check();  //creating new object
//error: as wrong datatype for second argument
var_dump( in_array("array_var", $in_array_obj) ); 
//error: as wrong datatype for second argument
var_dump( in_array("foo", $in_array_obj) ); 
//element found as "one" exists in array $array_var
var_dump( in_array("one", $in_array_obj->array_var) ); 

/* checking for Resources */
echo "\n*** Testing resource type with in_array() ***\n";
//file type resource
$file_handle = fopen(__FILE__, "r");

//directory type resource
$dir_handle = opendir( dirname(__FILE__) );

//store resources in array for comparision.
$resources = array($file_handle, $dir_handle);

// search for resouce type in the resource array
var_dump( in_array($file_handle, $resources, true) ); 
//checking for (int) type resource
var_dump( in_array((int)$dir_handle, $resources, true) ); 

/* Miscellenous input check  */
echo "\n*** Testing miscelleneos inputs with in_array() ***\n";
//matching "Good" in array(0,"hello"), result:true in loose type check
var_dump( in_array("Good", array(0,"hello")) ); 
//false in strict mode 
var_dump( in_array("Good", array(0,"hello"), TRUE) ); 

//matching integer 0 in array("this"), result:true in loose type check
var_dump( in_array(0, array("this")) );  
// false in strict mode
var_dump( in_array(0, array("this")),TRUE ); 

//matching string "this" in array(0), result:true in loose type check
var_dump( in_array("this", array(0)) );  
// false in stric mode
var_dump( in_array("this", array(0), TRUE) ); 

//checking for type FALSE in multidimensional array with loose checking, result:false in loose type check
var_dump( in_array(FALSE, 
                   array("a"=> TRUE, "b"=> TRUE, 
                         array("c"=> TRUE, "d"=>TRUE) 
                        ) 
                  ) 
        ); 

//matching string having integer in beginning, result:true in loose type check
var_dump( in_array('123abc', array(123)) );  
var_dump( in_array('123abc', array(123), TRUE) ); // false in strict mode 

echo "\n*** Testing error conditions of in_array() ***\n";
/* zero argument */
var_dump( in_array() );

/* unexpected no.of arguments in in_array() */
$var = array("mon", "tues", "wed", "thurs");
var_dump( in_array(1, $var, 0, "test") );
var_dump( in_array("test") );

/* unexpected second argument in in_array() */
$var="test";
var_dump( in_array("test", $var) );
var_dump( in_array(1, 123) );

/* closing resource handles */
fclose($file_handle); 
closedir($dir_handle); 

echo "Done\n";
?>
--EXPECTF--
OK
*** Testing STRICT option of in_array() on arrays ***
-- Iteration 1 --
bool(false)
bool(false)
bool(false)
-- Iteration 2 --
bool(false)
bool(false)
bool(false)
-- Iteration 3 --
bool(false)
bool(false)
bool(false)
-- Iteration 4 --
bool(true)
bool(false)
bool(true)
-- Iteration 5 --
bool(false)
bool(false)
bool(false)
-- Iteration 6 --
bool(false)
bool(false)
bool(false)
-- Iteration 7 --
bool(false)
bool(false)
bool(false)
-- Iteration 8 --
bool(false)
bool(false)
bool(false)
-- Iteration 9 --
bool(false)
bool(false)
bool(false)
-- Iteration 10 --
bool(true)
bool(false)
bool(true)
-- Iteration 11 --
bool(true)
bool(false)
bool(true)
-- Iteration 12 --
bool(false)
bool(false)
bool(false)
-- Iteration 13 --
bool(true)
bool(false)
bool(true)
-- Iteration 14 --
bool(true)
bool(false)
bool(true)
-- Iteration 15 --
bool(true)
bool(false)
bool(true)
-- Iteration 16 --
bool(true)
bool(false)
bool(true)
-- Iteration 17 --
bool(true)
bool(true)
bool(true)
-- Iteration 18 --
bool(true)
bool(false)
bool(true)
-- Iteration 19 --
bool(true)
bool(true)
bool(true)
-- Iteration 20 --
bool(true)
bool(false)
bool(true)
-- Iteration 21 --
bool(true)
bool(false)
bool(true)
-- Iteration 22 --
bool(true)
bool(true)
bool(true)
-- Iteration 23 --
bool(false)
bool(false)
bool(false)
-- Iteration 24 --
bool(false)
bool(false)
bool(false)
-- Iteration 25 --
bool(false)
bool(false)
bool(false)
-- Iteration 26 --
bool(false)
bool(false)
bool(false)
-- Iteration 27 --
bool(false)
bool(false)
bool(false)
-- Iteration 28 --
bool(false)
bool(false)
bool(false)
-- Iteration 29 --
bool(true)
bool(false)
bool(true)
-- Iteration 30 --
bool(true)
bool(false)
bool(true)
-- Iteration 31 --
bool(true)
bool(true)
bool(true)
-- Iteration 32 --
bool(true)
bool(true)
bool(true)
-- Iteration 33 --
bool(true)
bool(true)
bool(true)
-- Iteration 34 --
bool(true)
bool(false)
bool(true)
-- Iteration 35 --
bool(true)
bool(false)
bool(true)
-- Iteration 36 --
bool(false)
bool(false)
bool(false)
-- Iteration 37 --
bool(true)
bool(true)
bool(true)
-- Iteration 38 --
bool(true)
bool(false)
bool(true)
-- Iteration 39 --
bool(true)
bool(false)
bool(true)
-- Iteration 40 --
bool(false)
bool(false)
bool(false)
-- Iteration 41 --
bool(true)
bool(false)
bool(true)
-- Iteration 42 --
bool(false)
bool(false)
bool(false)
-- Iteration 43 --
bool(false)
bool(false)
bool(false)
-- Iteration 44 --
bool(false)
bool(false)
bool(false)
-- Iteration 45 --
bool(false)
bool(false)
bool(false)
-- Iteration 46 --
bool(false)
bool(false)
bool(false)
-- Iteration 47 --
bool(false)
bool(false)
bool(false)
-- Iteration 48 --
bool(false)
bool(false)
bool(false)
-- Iteration 49 --
bool(false)
bool(false)
bool(false)
-- Iteration 50 --
bool(false)
bool(false)
bool(false)
-- Iteration 51 --
bool(false)
bool(false)
bool(false)
-- Iteration 52 --
bool(false)
bool(false)
bool(false)
-- Iteration 53 --
bool(false)
bool(false)
bool(false)
-- Iteration 54 --
bool(false)
bool(false)
bool(false)
-- Iteration 55 --
bool(false)
bool(false)
bool(false)
-- Iteration 56 --
bool(false)
bool(false)
bool(false)
-- Iteration 57 --
bool(false)
bool(false)
bool(false)
-- Iteration 58 --
bool(false)
bool(false)
bool(false)
-- Iteration 59 --
bool(false)
bool(false)
bool(false)
-- Iteration 60 --
bool(true)
bool(true)
bool(true)
-- Iteration 61 --
bool(true)
bool(false)
bool(true)
-- Iteration 62 --
bool(false)
bool(false)
bool(false)
-- Iteration 63 --
bool(true)
bool(false)
bool(true)
-- Iteration 64 --
bool(false)
bool(false)
bool(false)
-- Iteration 65 --
bool(false)
bool(false)
bool(false)
-- Iteration 66 --
bool(false)
bool(false)
bool(false)
-- Iteration 67 --
bool(false)
bool(false)
bool(false)
-- Iteration 68 --
bool(false)
bool(false)
bool(false)
-- Iteration 69 --
bool(false)
bool(false)
bool(false)
-- Iteration 70 --
bool(true)
bool(false)
bool(true)
-- Iteration 71 --
bool(true)
bool(false)
bool(true)
-- Iteration 72 --
bool(false)
bool(false)
bool(false)
-- Iteration 73 --
bool(true)
bool(false)
bool(true)
-- Iteration 74 --
bool(true)
bool(false)
bool(true)
-- Iteration 75 --
bool(true)
bool(false)
bool(true)
-- Iteration 76 --
bool(true)
bool(false)
bool(true)
-- Iteration 77 --
bool(true)
bool(false)
bool(true)
-- Iteration 78 --
bool(true)
bool(false)
bool(true)
-- Iteration 79 --
bool(true)
bool(false)
bool(true)
-- Iteration 80 --
bool(true)
bool(false)
bool(true)
-- Iteration 81 --
bool(true)
bool(false)
bool(true)
-- Iteration 82 --
bool(true)
bool(false)
bool(true)
-- Iteration 83 --
bool(true)
bool(false)
bool(true)
-- Iteration 84 --
bool(true)
bool(false)
bool(true)
-- Iteration 85 --
bool(true)
bool(false)
bool(true)
-- Iteration 86 --
bool(true)
bool(false)
bool(true)
-- Iteration 87 --
bool(true)
bool(false)
bool(true)
-- Iteration 88 --
bool(true)
bool(false)
bool(true)
-- Iteration 89 --
bool(true)
bool(false)
bool(true)
-- Iteration 90 --
bool(true)
bool(false)
bool(true)
-- Iteration 91 --
bool(false)
bool(false)
bool(false)
-- Iteration 92 --
bool(false)
bool(false)
bool(false)
-- Iteration 93 --
bool(false)
bool(false)
bool(false)
-- Iteration 94 --
bool(false)
bool(false)
bool(false)
-- Iteration 95 --
bool(false)
bool(false)
bool(false)
-- Iteration 96 --
bool(false)
bool(false)
bool(false)
-- Iteration 97 --
bool(false)
bool(false)
bool(false)
-- Iteration 98 --
bool(false)
bool(false)
bool(false)
-- Iteration 99 --
bool(false)
bool(false)
bool(false)
-- Iteration 100 --
bool(false)
bool(false)
bool(false)
-- Iteration 101 --
bool(true)
bool(true)
bool(true)
-- Iteration 102 --
bool(true)
bool(true)
bool(true)
-- Iteration 103 --
bool(true)
bool(false)
bool(true)
-- Iteration 104 --
bool(false)
bool(false)
bool(false)
-- Iteration 105 --
bool(false)
bool(false)
bool(false)
-- Iteration 106 --
bool(true)
bool(false)
bool(true)
-- Iteration 107 --
bool(true)
bool(false)
bool(true)
-- Iteration 108 --
bool(false)
bool(false)
bool(false)
-- Iteration 109 --
bool(false)
bool(false)
bool(false)
-- Iteration 110 --
bool(false)
bool(false)
bool(false)
-- Iteration 111 --
bool(false)
bool(false)
bool(false)
-- Iteration 112 --
bool(false)
bool(false)
bool(false)
-- Iteration 113 --
bool(false)
bool(false)
bool(false)
-- Iteration 114 --
bool(false)
bool(false)
bool(false)
-- Iteration 115 --
bool(false)
bool(false)
bool(false)
-- Iteration 116 --
bool(false)
bool(false)
bool(false)
-- Iteration 117 --
bool(false)
bool(false)
bool(false)
-- Iteration 118 --
bool(false)
bool(false)
bool(false)
-- Iteration 119 --
bool(false)
bool(false)
bool(false)
-- Iteration 120 --
bool(false)
bool(false)
bool(false)
-- Iteration 121 --
bool(false)
bool(false)
bool(false)
-- Iteration 122 --
bool(false)
bool(false)
bool(false)
-- Iteration 123 --
bool(false)
bool(false)
bool(false)
-- Iteration 124 --
bool(true)
bool(false)
bool(true)
-- Iteration 125 --
bool(true)
bool(false)
bool(true)
-- Iteration 126 --
bool(true)
bool(true)
bool(true)
-- Iteration 127 --
bool(false)
bool(false)
bool(false)
-- Iteration 128 --
bool(false)
bool(false)
bool(false)
-- Iteration 129 --
bool(false)
bool(false)
bool(false)
-- Iteration 130 --
bool(false)
bool(false)
bool(false)
-- Iteration 131 --
bool(false)
bool(false)
bool(false)
-- Iteration 132 --
bool(false)
bool(false)
bool(false)
-- Iteration 133 --
bool(false)
bool(false)
bool(false)
-- Iteration 134 --
bool(false)
bool(false)
bool(false)
-- Iteration 135 --
bool(false)
bool(false)
bool(false)
-- Iteration 136 --
bool(false)
bool(false)
bool(false)
-- Iteration 137 --
bool(false)
bool(false)
bool(false)
-- Iteration 138 --
bool(false)
bool(false)
bool(false)
-- Iteration 139 --
bool(false)
bool(false)
bool(false)
-- Iteration 140 --
bool(false)
bool(false)
bool(false)
-- Iteration 141 --
bool(false)
bool(false)
bool(false)
-- Iteration 142 --
bool(true)
bool(false)
bool(true)
-- Iteration 143 --
bool(true)
bool(false)
bool(true)
-- Iteration 144 --
bool(false)
bool(false)
bool(false)

*** Testing loose and strict TYPE comparison of in_array() ***
-- Iteration 1 --
bool(true)
bool(true)
bool(true)
-- Iteration 2 --
bool(true)
bool(true)
bool(true)
-- Iteration 3 --
bool(true)
bool(false)
bool(true)
-- Iteration 4 --
bool(true)
bool(true)
bool(true)
-- Iteration 5 --
bool(true)
bool(false)
bool(true)
-- Iteration 6 --
bool(true)
bool(false)
bool(true)
-- Iteration 7 --
bool(true)
bool(false)
bool(true)
-- Iteration 8 --
bool(true)
bool(false)
bool(true)
-- Iteration 9 --
bool(true)
bool(true)
bool(true)
-- Iteration 10 --
bool(true)
bool(false)
bool(true)
-- Iteration 11 --
bool(true)
bool(false)
bool(true)
-- Iteration 12 --
bool(true)
bool(true)
bool(true)

*** Testing sub-arrays with in_array() ***
bool(true)
bool(true)
bool(true)

*** Testing objects with in_array() ***

Warning: in_array(): Wrong datatype for second argument in %s on line %d
bool(false)

Warning: in_array(): Wrong datatype for second argument in %s on line %d
bool(false)
bool(true)

*** Testing resource type with in_array() ***
bool(true)
bool(false)

*** Testing miscelleneos inputs with in_array() ***
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)

*** Testing error conditions of in_array() ***

Warning: Wrong parameter count for in_array() in %s on line %d
NULL

Warning: Wrong parameter count for in_array() in %s on line %d
NULL

Warning: Wrong parameter count for in_array() in %s on line %d
NULL

Warning: in_array(): Wrong datatype for second argument in %s on line %d
bool(false)

Warning: in_array(): Wrong datatype for second argument in %s on line %d
bool(false)
Done
