--TEST--
Test natcasesort() function : object functionality - mixed visibility within objects
--FILE--
<?php
/*
 * Pass natcasesort() an array of objects which have properties of different
 * visibilities to test how it re-orders the array.
 */

echo "*** Testing natcasesort() : object functionality ***\n";

// class declaration for string objects
class for_string_natcasesort
{
    public $public_class_value;
    private $private_class_value;
    protected $protected_class_value;
    // initializing object member value
    function __construct($value1, $value2,$value3){
        $this->public_class_value = $value1;
        $this->private_class_value = $value2;
        $this->protected_class_value = $value3;
    }

    // return string value
    function __tostring() {
        return (string)$this->public_class_value;
    }

}

// array of string objects
$unsorted_str_obj = array (
new for_string_natcasesort("axx","AXX","ass"),
new for_string_natcasesort("t","eee","abb"),
new for_string_natcasesort("w","W", "c"),
new for_string_natcasesort("py","PY", "pt"),
);


echo "\n-- Testing natcasesort() by supplying object arrays --\n";

// testing natcasesort() function by supplying string object array
$temp_array = $unsorted_str_obj;
var_dump(natcasesort($temp_array) );
var_dump($temp_array);

echo "Done";
?>
--EXPECTF--
*** Testing natcasesort() : object functionality ***

-- Testing natcasesort() by supplying object arrays --
bool(true)
array(4) {
  [0]=>
  object(for_string_natcasesort)#%d (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value":"for_string_natcasesort":private]=>
    string(3) "AXX"
    ["protected_class_value":protected]=>
    string(3) "ass"
  }
  [3]=>
  object(for_string_natcasesort)#%d (3) {
    ["public_class_value"]=>
    string(2) "py"
    ["private_class_value":"for_string_natcasesort":private]=>
    string(2) "PY"
    ["protected_class_value":protected]=>
    string(2) "pt"
  }
  [1]=>
  object(for_string_natcasesort)#%d (3) {
    ["public_class_value"]=>
    string(1) "t"
    ["private_class_value":"for_string_natcasesort":private]=>
    string(3) "eee"
    ["protected_class_value":protected]=>
    string(3) "abb"
  }
  [2]=>
  object(for_string_natcasesort)#%d (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value":"for_string_natcasesort":private]=>
    string(1) "W"
    ["protected_class_value":protected]=>
    string(1) "c"
  }
}
Done
