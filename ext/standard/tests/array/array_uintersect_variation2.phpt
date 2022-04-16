--TEST--
Test array_uintersect() function : usage variation
--FILE--
<?php
echo "*** Testing array_uintersect() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$array1 = array(1, 2);

include('compare_function.inc');
$data_compare_function = 'compare_function';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// define some classes
class classWithToString
{
    public function __toString() {
        return "Class A object";
    }
}

class classWithoutToString
{
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// add arrays
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

//array of values to iterate over
$inputs = array(

      // int data
      'int 0' => 0,
      'int 1' => 1,
      'int 12345' => 12345,
      'int -12345' => -2345,

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
      'float 12.3456789000e10' => 12.3456789000e10,
      'float -12.3456789000e10' => -12.3456789000e10,
      'float .5' => .5,

      // null data
      'uppercase NULL' => NULL,
      'lowercase null' => null,

      // boolean data
      'lowercase true' => true,
      'lowercase false' =>false,
      'uppercase TRUE' =>TRUE,
      'uppercase FALSE' =>FALSE,

      // empty data
      'empty string DQ' => "",
      'empty string SQ' => '',

      // string data
      'string DQ' => "string",
      'string SQ' => 'string',
      'mixed case string' => "sTrInG",
      'heredoc' => $heredoc,

      // object data
      'instance of classWithToString' => new classWithToString(),
      'instance of classWithoutToString' => new classWithoutToString(),

      // undefined data
      'undefined var' => @$undefined_var,

      // unset data
      'unset var' => @$unset_var,
);

// loop through each element of the array for array2

foreach($inputs as $key =>$value) {
    echo "\n--$key--\n";
    try {
        var_dump( array_uintersect($array1, $value, $data_compare_function) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
};

?>
--EXPECT--
*** Testing array_uintersect() : usage variation ***

--int 0--
array_uintersect(): Argument #2 must be of type array, int given

--int 1--
array_uintersect(): Argument #2 must be of type array, int given

--int 12345--
array_uintersect(): Argument #2 must be of type array, int given

--int -12345--
array_uintersect(): Argument #2 must be of type array, int given

--float 10.5--
array_uintersect(): Argument #2 must be of type array, float given

--float -10.5--
array_uintersect(): Argument #2 must be of type array, float given

--float 12.3456789000e10--
array_uintersect(): Argument #2 must be of type array, float given

--float -12.3456789000e10--
array_uintersect(): Argument #2 must be of type array, float given

--float .5--
array_uintersect(): Argument #2 must be of type array, float given

--uppercase NULL--
array_uintersect(): Argument #2 must be of type array, null given

--lowercase null--
array_uintersect(): Argument #2 must be of type array, null given

--lowercase true--
array_uintersect(): Argument #2 must be of type array, true given

--lowercase false--
array_uintersect(): Argument #2 must be of type array, false given

--uppercase TRUE--
array_uintersect(): Argument #2 must be of type array, true given

--uppercase FALSE--
array_uintersect(): Argument #2 must be of type array, false given

--empty string DQ--
array_uintersect(): Argument #2 must be of type array, string given

--empty string SQ--
array_uintersect(): Argument #2 must be of type array, string given

--string DQ--
array_uintersect(): Argument #2 must be of type array, string given

--string SQ--
array_uintersect(): Argument #2 must be of type array, string given

--mixed case string--
array_uintersect(): Argument #2 must be of type array, string given

--heredoc--
array_uintersect(): Argument #2 must be of type array, string given

--instance of classWithToString--
array_uintersect(): Argument #2 must be of type array, classWithToString given

--instance of classWithoutToString--
array_uintersect(): Argument #2 must be of type array, classWithoutToString given

--undefined var--
array_uintersect(): Argument #2 must be of type array, null given

--unset var--
array_uintersect(): Argument #2 must be of type array, null given
