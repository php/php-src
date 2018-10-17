--TEST--
Test arsort() function : object functionality - sort objects
--FILE--
<?php
/* Prototype  : bool arsort ( array &$array [, int $asort_flags] )
 * Description: Sort an array and maintain index association.
                Elements will be arranged from highest to lowest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing arsort() by providing integer/string object arrays with following flag values
 * 1. Defualt flag value
 * 2. SORT_REGULAR - compare items normally
*/

echo "*** Testing arsort() : object functionality ***\n";

// class declaration for integer objects
class for_integer_arsort
{
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
  }

}

// class declaration for string objects
class for_string_arsort
{
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
   }

  // return string value
  function __tostring() {
   return (string)$this->value;
  }

}

// array of integer objects
$unsorted_int_obj = array (
  1 => new for_integer_arsort(11), 2 =>  new for_integer_asort(66),
  3 => new for_integer_arsort(23), 4 => new for_integer_asort(-5),
  5 => new for_integer_arsort(0.001), 6 => new for_integer_asort(0)
);

// array of string objects
$unsorted_str_obj = array (
  "a" => new for_string_arsort("axx"), "b" => new for_string_asort("t"),
  "c" => new for_string_arsort("w"), "d" => new for_string_asort("py"),
  "e" => new for_string_arsort("apple"), "f" => new for_string_asort("Orange"),
  "g" => new for_string_arsort("Lemon"), "h" => new for_string_asort("aPPle")
);


echo "\n-- Testing arsort() by supplying various object arrays, 'flag' value is defualt --\n";

// testing arsort() function by supplying integer object array, flag value is defualt
$temp_array = $unsorted_int_obj;
var_dump(arsort($temp_array) );
var_dump($temp_array);

// testing arsort() function by supplying string object array, flag value is defualt
$temp_array = $unsorted_str_obj;
var_dump(arsort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying various object arrays, 'flag' value is SORT_REGULAR --\n";
// testing arsort() function by supplying integer object array, flag value = SORT_REGULAR
$temp_array = $unsorted_int_obj;
var_dump(arsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

// testing arsort() function by supplying string object array, flag value = SORT_REGULAR
$temp_array = $unsorted_str_obj;
var_dump(arsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing arsort() : object functionality ***

Fatal error: Uncaught Error: Class 'for_integer_asort' not found in %sarsort_object1.php:%d
Stack trace:
#0 {main}
  thrown in %sarsort_object1.php on line %d
