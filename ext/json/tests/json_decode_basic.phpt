--TEST--
Test json_decode() function : basic functionality
--SKIPIF--
<?php
if (!extension_loaded("json")) {
 	die('skip JSON extension not available in this build');
}	 
?>
--FILE--
<?php
/* Prototype  : mixed json_decode  ( string $json  [, bool $assoc  ] )
 * Description: Decodes a JSON string
 * Source code: ext/json/php_json.c
 * Alias to functions: 
 */
echo "*** Testing json_decode() : basic functionality ***\n";

// array with different values for $string
$inputs =  array (
		'0',
		'123',
		'-123',
		'2147483647',
		'-2147483648',
		'123.456',
		'1230',
		'-1230',
		'true',
		'false',
		'null',
		'"abc"',
		'"Hello World\r\n"',
		'[]',
		'[1,2,3,4,5]',
		'{"myInt":99,"myFloat":123.45,"myNull":null,"myBool":true,"myString":"Hello World"}',
		'{"Jan":31,"Feb":29,"Mar":31,"April":30,"May":31,"June":30}',
		'""',
		'{}'
);  

// loop through with each element of the $inputs array to test json_decode() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";	
  var_dump(json_decode($input)); 
   var_dump(json_decode($input, TRUE)); 
  $count ++;
}

?>
===DONE===
--EXPECTF-- 
*** Testing json_decode() : basic functionality ***
-- Iteration 1 --
int(0)
int(0)
-- Iteration 2 --
int(123)
int(123)
-- Iteration 3 --
int(-123)
int(-123)
-- Iteration 4 --
int(2147483647)
int(2147483647)
-- Iteration 5 --
int(-2147483648)
int(-2147483648)
-- Iteration 6 --
float(123.456)
float(123.456)
-- Iteration 7 --
int(1230)
int(1230)
-- Iteration 8 --
int(-1230)
int(-1230)
-- Iteration 9 --
bool(true)
bool(true)
-- Iteration 10 --
bool(false)
bool(false)
-- Iteration 11 --
NULL
NULL
-- Iteration 12 --
unicode(3) "abc"
unicode(3) "abc"
-- Iteration 13 --
unicode(13) "Hello World
"
unicode(13) "Hello World
"
-- Iteration 14 --
array(0) {
}
array(0) {
}
-- Iteration 15 --
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
-- Iteration 16 --
object(stdClass)#%d (5) {
  [u"myInt"]=>
  int(99)
  [u"myFloat"]=>
  float(123.45)
  [u"myNull"]=>
  NULL
  [u"myBool"]=>
  bool(true)
  [u"myString"]=>
  unicode(11) "Hello World"
}
array(5) {
  [u"myInt"]=>
  int(99)
  [u"myFloat"]=>
  float(123.45)
  [u"myNull"]=>
  NULL
  [u"myBool"]=>
  bool(true)
  [u"myString"]=>
  unicode(11) "Hello World"
}
-- Iteration 17 --
object(stdClass)#%d (6) {
  [u"Jan"]=>
  int(31)
  [u"Feb"]=>
  int(29)
  [u"Mar"]=>
  int(31)
  [u"April"]=>
  int(30)
  [u"May"]=>
  int(31)
  [u"June"]=>
  int(30)
}
array(6) {
  [u"Jan"]=>
  int(31)
  [u"Feb"]=>
  int(29)
  [u"Mar"]=>
  int(31)
  [u"April"]=>
  int(30)
  [u"May"]=>
  int(31)
  [u"June"]=>
  int(30)
}
-- Iteration 18 --
unicode(0) ""
unicode(0) ""
-- Iteration 19 --
object(stdClass)#%d (0) {
}
array(0) {
}
===DONE===
