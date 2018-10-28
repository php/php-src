--TEST--
Test getdate() function : usage variation - Verifyig with different timezones on Unix epoch timestamp
--FILE--
<?php
/* Prototype  : array getdate([int timestamp])
 * Description: Get date/time information
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing getdate() : usage variation ***\n";

//Timezones with required data for date_sunrise
$inputs = array (
		//GMT-11
		"Pacific/Samoa",
		//GMT-9
		"US/Alaska",
		//GMT-0
		"Africa/Casablanca",
		//GMT+4
		"Europe/Moscow",
		//GMT+8
		"Asia/Hong_Kong",
		//GMT+10
		"Australia/Brisbane",
		//GMT+12
		"Pacific/Wallis",
);

// loop through each element of the array for timestamp
foreach($inputs as $timezone) {
      echo "\n--$timezone--\n";
      date_default_timezone_set($timezone);
      var_dump( getdate(0) );
};
?>
===DONE===
--EXPECT--
*** Testing getdate() : usage variation ***

--Pacific/Samoa--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(0)
  ["hours"]=>
  int(13)
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
  int(0)
}

--US/Alaska--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(0)
  ["hours"]=>
  int(14)
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
  int(0)
}

--Africa/Casablanca--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(0)
  ["hours"]=>
  int(0)
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

--Europe/Moscow--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(0)
  ["hours"]=>
  int(3)
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

--Asia/Hong_Kong--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(0)
  ["hours"]=>
  int(8)
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

--Australia/Brisbane--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(0)
  ["hours"]=>
  int(10)
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

--Pacific/Wallis--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(0)
  ["hours"]=>
  int(12)
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
