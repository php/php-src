--TEST--
Test getdate() function : usage variation - Passing octal timestamp values
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

//array of values to iterate over
$inputs = array(

	//octal values
	'octal 05' => 05,
	'octal 010' => 010,
	'octal -010' => -010,
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

--octal 05--
array(11) {
  ["seconds"]=>
  int(5)
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
  int(5)
}

--octal 010--
array(11) {
  ["seconds"]=>
  int(8)
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
  int(8)
}

--octal -010--
array(11) {
  ["seconds"]=>
  int(52)
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
  int(-8)
}
===DONE===
