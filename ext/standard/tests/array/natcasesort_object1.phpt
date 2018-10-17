--TEST--
Test natcasesort() function : object functionality - array of objects
--FILE--
<?php
/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Pass natcasesort() an array of objects to test how it re-orders them
 */

echo "*** Testing natcasesort() : object functionality ***\n";

// class declaration for string objects
class for_string_natcasesort
{
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
   }

  // return string value
  function __tostring() {
   return (string)$this->class_value;
  }

}



// array of string objects
$unsorted_str_obj = array (
  new for_string_natcasesort("axx"), new for_string_natcasesort("t"),
  new for_string_natcasesort("w"), new for_string_natcasesort("py"),
  new for_string_natcasesort("apple"), new for_string_natcasesort("Orange"),
  new for_string_natcasesort("Lemon"), new for_string_natcasesort("aPPle")
);


echo "\n-- Testing natcasesort() by supplying various object arrays --\n";

// testing natcasesort() function by supplying string object array
var_dump(natcasesort($unsorted_str_obj) );
var_dump($unsorted_str_obj);

echo "Done";
?>
--EXPECTF--
*** Testing natcasesort() : object functionality ***

-- Testing natcasesort() by supplying various object arrays --
bool(true)
array(8) {
  [4]=>
  object(for_string_natcasesort)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  [7]=>
  object(for_string_natcasesort)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  [0]=>
  object(for_string_natcasesort)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  [6]=>
  object(for_string_natcasesort)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
  [5]=>
  object(for_string_natcasesort)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  [3]=>
  object(for_string_natcasesort)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  [1]=>
  object(for_string_natcasesort)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  [2]=>
  object(for_string_natcasesort)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
}
Done
