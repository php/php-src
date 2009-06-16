--TEST--
Test interface_exists() function : usage variation 
--FILE--
<?php
/* Prototype  : bool interface_exists(string classname [, bool autoload])
 * Description: Checks if the class exists 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing interface_exists() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$classname = 'aBogusInterfaceName';

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

      // array data
      'empty array' => array(),
      'int indexed array' => $index_array,
      'associative array' => $assoc_array,
      'nested arrays' => array('foo', $index_array, $assoc_array),

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

// loop through each element of the array for autoload

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( interface_exists($classname, $value) );
};

?>
===DONE===
--EXPECTF--
*** Testing interface_exists() : usage variation ***

--int 0--
bool(false)

--int 1--
bool(false)

--int 12345--
bool(false)

--int -12345--
bool(false)

--float 10.5--
bool(false)

--float -10.5--
bool(false)

--float 12.3456789000e10--
bool(false)

--float -12.3456789000e10--
bool(false)

--float .5--
bool(false)

--empty array--

Warning: interface_exists() expects parameter 2 to be boolean, array given in %sinterface_exists_variation2.php on line %d
NULL

--int indexed array--

Warning: interface_exists() expects parameter 2 to be boolean, array given in %sinterface_exists_variation2.php on line %d
NULL

--associative array--

Warning: interface_exists() expects parameter 2 to be boolean, array given in %sinterface_exists_variation2.php on line %d
NULL

--nested arrays--

Warning: interface_exists() expects parameter 2 to be boolean, array given in %sinterface_exists_variation2.php on line %d
NULL

--uppercase NULL--
bool(false)

--lowercase null--
bool(false)

--lowercase true--
bool(false)

--lowercase false--
bool(false)

--uppercase TRUE--
bool(false)

--uppercase FALSE--
bool(false)

--empty string DQ--
bool(false)

--empty string SQ--
bool(false)

--string DQ--
bool(false)

--string SQ--
bool(false)

--mixed case string--
bool(false)

--heredoc--
bool(false)

--instance of classWithToString--

Warning: interface_exists() expects parameter 2 to be boolean, object given in %sinterface_exists_variation2.php on line %d
NULL

--instance of classWithoutToString--

Warning: interface_exists() expects parameter 2 to be boolean, object given in %sinterface_exists_variation2.php on line %d
NULL

--undefined var--
bool(false)

--unset var--
bool(false)
===DONE===
