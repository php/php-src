--TEST--
Test array_intersect_uassoc() function : usage variation - Passing unexpected values to second argument
--FILE--
<?php
echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

// Initialise function arguments
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array3 = array("a"=>"green", "brown");

//Callback function
function key_compare_func($a, $b) {
    if ($a === $b) {
        return 0;
    }
    return ($a > $b) ? 1 : -1;
}

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

// add arrays
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

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
        var_dump( array_intersect_uassoc($array1, $value, 'key_compare_func') );
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        var_dump( array_intersect_uassoc($array1, $value, $array3, 'key_compare_func') );
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
};

fclose($fp);
?>
--EXPECT--
*** Testing array_intersect_uassoc() : usage variation ***

--int 0--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, int given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, int given

--int 1--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, int given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, int given

--int 12345--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, int given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, int given

--int -12345--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, int given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, int given

--float 10.5--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given

--float -10.5--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given

--float 12.3456789000e10--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given

--float -12.3456789000e10--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given

--float .5--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, float given

--uppercase NULL--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, null given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, null given

--lowercase null--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, null given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, null given

--lowercase true--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, true given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, true given

--lowercase false--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, false given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, false given

--uppercase TRUE--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, true given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, true given

--uppercase FALSE--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, false given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, false given

--empty string DQ--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given

--empty string SQ--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given

--string DQ--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given

--string SQ--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given

--mixed case string--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given

--heredoc--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, string given

--instance of classWithToString--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, classWithToString given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, classWithToString given

--instance of classWithoutToString--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, classWithoutToString given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, classWithoutToString given

--undefined var--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, null given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, null given

--unset var--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, null given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, null given

--resource--
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, resource given
TypeError: array_intersect_uassoc(): Argument #2 must be of type array, resource given
