--TEST--
Test getdate() function : usage variation - Passing hexadcimal timestamp values
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
	'hexadcimal 0x5' => 0x5,
	'hexadcimal 0xCAFE' => 0xCAFE,
	'octal -0xCAFE' => -0xCAFE,
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( getdate($value) );
};

?>
===DONE===
--EXPECT--
*** Testing getdate() : usage variation ***

--hexadcimal 0x5--
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

--hexadcimal 0xCAFE--
array(11) {
  ["seconds"]=>
  int(6)
  ["minutes"]=>
  int(56)
  ["hours"]=>
  int(19)
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
  int(51966)
}

--octal -0xCAFE--
array(11) {
  ["seconds"]=>
  int(54)
  ["minutes"]=>
  int(3)
  ["hours"]=>
  int(15)
  ["mday"]=>
  int(31)
  ["wday"]=>
  int(3)
  ["mon"]=>
  int(12)
  ["year"]=>
  int(1969)
  ["yday"]=>
  int(364)
  ["weekday"]=>
  string(9) "Wednesday"
  ["month"]=>
  string(8) "December"
  [0]=>
  int(-51966)
}
===DONE===
