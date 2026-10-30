--TEST--
Test array_diff_key() function : usage variation - Passing unexpected values to second argument
--FILE--
<?php
echo "*** Testing array_diff_key() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$array1 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan' => 8);
$array3 = array(1, 2, 3, 4, 5);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//resource variable
$fp = fopen(__FILE__, "r");

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

//array of values to iterate over
$inputs = array(

      // int data
      'int 0' => 0,
      'int 1' => 1,
      'int 12345' => 12345,
      'int -12345' => -12345,

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

      // resource data
      'resource' => $fp,
);

// loop through each element of the array for array1

foreach($inputs as $key =>$value) {
    echo "\n--$key--\n";
    try {
        var_dump( array_diff_key($array1, $value) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        var_dump( array_diff_key($array1, $value, $array3) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
};

fclose($fp);
?>
--EXPECT--
*** Testing array_diff_key() : usage variation ***

--int 0--
array_diff_key(): Argument #2 must be of type array, int given
array_diff_key(): Argument #2 must be of type array, int given

--int 1--
array_diff_key(): Argument #2 must be of type array, int given
array_diff_key(): Argument #2 must be of type array, int given

--int 12345--
array_diff_key(): Argument #2 must be of type array, int given
array_diff_key(): Argument #2 must be of type array, int given

--int -12345--
array_diff_key(): Argument #2 must be of type array, int given
array_diff_key(): Argument #2 must be of type array, int given

--float 10.5--
array_diff_key(): Argument #2 must be of type array, float given
array_diff_key(): Argument #2 must be of type array, float given

--float -10.5--
array_diff_key(): Argument #2 must be of type array, float given
array_diff_key(): Argument #2 must be of type array, float given

--float 12.3456789000e10--
array_diff_key(): Argument #2 must be of type array, float given
array_diff_key(): Argument #2 must be of type array, float given

--float -12.3456789000e10--
array_diff_key(): Argument #2 must be of type array, float given
array_diff_key(): Argument #2 must be of type array, float given

--float .5--
array_diff_key(): Argument #2 must be of type array, float given
array_diff_key(): Argument #2 must be of type array, float given

--uppercase NULL--
array_diff_key(): Argument #2 must be of type array, null given
array_diff_key(): Argument #2 must be of type array, null given

--lowercase null--
array_diff_key(): Argument #2 must be of type array, null given
array_diff_key(): Argument #2 must be of type array, null given

--lowercase true--
array_diff_key(): Argument #2 must be of type array, true given
array_diff_key(): Argument #2 must be of type array, true given

--lowercase false--
array_diff_key(): Argument #2 must be of type array, false given
array_diff_key(): Argument #2 must be of type array, false given

--uppercase TRUE--
array_diff_key(): Argument #2 must be of type array, true given
array_diff_key(): Argument #2 must be of type array, true given

--uppercase FALSE--
array_diff_key(): Argument #2 must be of type array, false given
array_diff_key(): Argument #2 must be of type array, false given

--empty string DQ--
array_diff_key(): Argument #2 must be of type array, string given
array_diff_key(): Argument #2 must be of type array, string given

--empty string SQ--
array_diff_key(): Argument #2 must be of type array, string given
array_diff_key(): Argument #2 must be of type array, string given

--string DQ--
array_diff_key(): Argument #2 must be of type array, string given
array_diff_key(): Argument #2 must be of type array, string given

--string SQ--
array_diff_key(): Argument #2 must be of type array, string given
array_diff_key(): Argument #2 must be of type array, string given

--mixed case string--
array_diff_key(): Argument #2 must be of type array, string given
array_diff_key(): Argument #2 must be of type array, string given

--heredoc--
array_diff_key(): Argument #2 must be of type array, string given
array_diff_key(): Argument #2 must be of type array, string given

--instance of classWithToString--
array_diff_key(): Argument #2 must be of type array, classWithToString given
array_diff_key(): Argument #2 must be of type array, classWithToString given

--instance of classWithoutToString--
array_diff_key(): Argument #2 must be of type array, classWithoutToString given
array_diff_key(): Argument #2 must be of type array, classWithoutToString given

--undefined var--
array_diff_key(): Argument #2 must be of type array, null given
array_diff_key(): Argument #2 must be of type array, null given

--unset var--
array_diff_key(): Argument #2 must be of type array, null given
array_diff_key(): Argument #2 must be of type array, null given

--resource--
array_diff_key(): Argument #2 must be of type array, resource given
array_diff_key(): Argument #2 must be of type array, resource given
