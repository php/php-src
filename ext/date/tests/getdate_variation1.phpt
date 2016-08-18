--TEST--
Test getdate() function : usage variation - Passing unexpected values to first argument timestamp.
--FILE--
<?php
/* Prototype  : array getdate([int timestamp])
 * Description: Get date/time information 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing getdate() : usage variation ***\n";

//Set the default time zone 
date_default_timezone_set("Asia/Calcutta");

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

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
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

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( getdate($value) );
};

?>
===DONE===
--EXPECTF--
*** Testing getdate() : usage variation ***

--float 10.5--
array(11) {
  ["seconds"]=>
  int(10)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(10)
}

--float -10.5--
array(11) {
  ["seconds"]=>
  int(50)
  ["minutes"]=>
  int(29)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(-10)
}

--float .5--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(0)
}

--empty array--

Warning: getdate() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

--int indexed array--

Warning: getdate() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

--associative array--

Warning: getdate() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

--nested arrays--

Warning: getdate() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

--uppercase NULL--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(0)
}

--lowercase null--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(0)
}

--lowercase true--
array(11) {
  ["seconds"]=>
  int(1)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(1)
}

--lowercase false--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(0)
}

--uppercase TRUE--
array(11) {
  ["seconds"]=>
  int(1)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(1)
}

--uppercase FALSE--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(0)
}

--empty string DQ--

Warning: getdate() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: getdate() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

--string DQ--

Warning: getdate() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

--string SQ--

Warning: getdate() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

--mixed case string--

Warning: getdate() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

--heredoc--

Warning: getdate() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: getdate() expects parameter 1 to be integer, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: getdate() expects parameter 1 to be integer, object given in %s on line %d
bool(false)

--undefined var--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(0)
}

--unset var--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(0)
}
===DONE===
