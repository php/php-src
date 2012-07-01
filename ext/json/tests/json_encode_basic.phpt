--TEST--
Test json_encode() function : basic functionality
--SKIPIF--
<?php
if (!extension_loaded("json")) {
 	die('skip JSON extension not available in this build');
}	 
?>
--FILE--
<?php
/* Prototype  : string json_encode  ( mixed $value  )
 * Description: Returns the JSON representation of a value
 * Source code: ext/json/php_json.c
 * Alias to functions: 
 */
echo "*** Testing json_encode() : basic functionality ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a resource variable
$fp = fopen(__FILE__, "r");

// get an object
class sample  {
}

$obj = new sample();
$obj->MyInt = 99;
$obj->MyFloat = 123.45;
$obj->MyBool = true;
$obj->MyNull = null;
$obj->MyString = "Hello World";

// array with different values for $string
$inputs =  array (

		// integers
/*1*/	0,
  		123,
  		-123,
 		 2147483647,
  		-2147483648,
  
  		// floats 
/*6*/	123.456,
  		1.23E3,
  		-1.23E3,
  
 		 // boolean
/*9*/	TRUE,
  		true,
  		FALSE,
  		false,

		// NULL
/*13*/	NULL,
  		null,
  
  		// strings
/*15*/	"abc",
        'abc', 
  		"Hello\t\tWorld\n",
  
  		// arrays
/*18*/	array(),
  		array(1,2,3,4,5),
 		array(1 => "Sun", 2=>"Mon", 3 => "Tue", 4 => "Wed", 5 => "Thur", 6 => "Fri", 7 => "Sat"),
  		array("Jan" => 31, "Feb" => 29, "Mar" => 31, "April" => 30, "May" => 31, "June" => 30),
  
       // empty data
/*22*/ "",
       '',

       // undefined data
/*24*/ @$undefined_var,

       // unset data
/*25*/ @$unset_var,

       // resource variable
/*26*/ $fp,

	  // object variable
/*27*/ $obj 
  
);  

// loop through with each element of the $inputs array to test json_encode() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";	
  var_dump(json_encode($input)); 
  $count ++;
}

?>
===Done===
--EXPECTF-- 
*** Testing json_encode() : basic functionality ***
-- Iteration 1 --
string(1) "0"
-- Iteration 2 --
string(3) "123"
-- Iteration 3 --
string(4) "-123"
-- Iteration 4 --
string(10) "2147483647"
-- Iteration 5 --
string(11) "-2147483648"
-- Iteration 6 --
string(7) "123.456"
-- Iteration 7 --
string(4) "1230"
-- Iteration 8 --
string(5) "-1230"
-- Iteration 9 --
string(4) "true"
-- Iteration 10 --
string(4) "true"
-- Iteration 11 --
string(5) "false"
-- Iteration 12 --
string(5) "false"
-- Iteration 13 --
string(4) "null"
-- Iteration 14 --
string(4) "null"
-- Iteration 15 --
string(5) ""abc""
-- Iteration 16 --
string(5) ""abc""
-- Iteration 17 --
string(18) ""Hello\t\tWorld\n""
-- Iteration 18 --
string(2) "[]"
-- Iteration 19 --
string(11) "[1,2,3,4,5]"
-- Iteration 20 --
string(72) "{"1":"Sun","2":"Mon","3":"Tue","4":"Wed","5":"Thur","6":"Fri","7":"Sat"}"
-- Iteration 21 --
string(58) "{"Jan":31,"Feb":29,"Mar":31,"April":30,"May":31,"June":30}"
-- Iteration 22 --
string(2) """"
-- Iteration 23 --
string(2) """"
-- Iteration 24 --
string(4) "null"
-- Iteration 25 --
string(4) "null"
-- Iteration 26 --

Warning: json_encode(): type is unsupported, encoded as null in %s on line %d
string(4) "null"
-- Iteration 27 --
string(82) "{"MyInt":99,"MyFloat":123.45,"MyBool":true,"MyNull":null,"MyString":"Hello World"}"
===Done===
