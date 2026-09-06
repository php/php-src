--TEST--
Test array_multisort() function : usage variation
--FILE--
<?php
echo "*** Testing array_multisort() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum) {
    if (error_reporting() & $err_no) {
        // report non-silenced errors
        echo "Error: $err_no - $err_msg, $filename($linenum)\n";
    }
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$ar1 = array(1, 2);

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

// loop through each element of the array for ar2
foreach($inputs as $key =>$value) {
    echo "\n--$key--\n";
    try {
        var_dump( array_multisort($ar1, SORT_REGULAR, $value) );
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
};

?>
--EXPECT--
*** Testing array_multisort() : usage variation ***

--int 0--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag that has not already been specified

--int 1--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag that has not already been specified

--int 12345--
ValueError: array_multisort(): Argument #3 must be a valid sort flag

--int -12345--
ValueError: array_multisort(): Argument #3 must be a valid sort flag

--float 10.5--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--float -10.5--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--float 12.3456789000e10--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--float -12.3456789000e10--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--float .5--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--uppercase NULL--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--lowercase null--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--lowercase true--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--lowercase false--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--uppercase TRUE--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--uppercase FALSE--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--empty string DQ--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--empty string SQ--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--string DQ--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--string SQ--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--mixed case string--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--heredoc--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--instance of classWithToString--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--instance of classWithoutToString--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--undefined var--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag

--unset var--
TypeError: array_multisort(): Argument #3 must be an array or a sort flag
