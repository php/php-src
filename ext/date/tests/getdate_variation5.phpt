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
--EXPECTF--
*** Testing getdate() : usage variation ***

--Pacific/Samoa--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(0)
  [u"hours"]=>
  int(13)
  [u"mday"]=>
  int(31)
  [u"wday"]=>
  int(3)
  [u"mon"]=>
  int(12)
  [u"year"]=>
  int(1969)
  [u"yday"]=>
  int(364)
  [u"weekday"]=>
  unicode(9) "Wednesday"
  [u"month"]=>
  unicode(8) "December"
  [0]=>
  int(0)
}

--US/Alaska--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(0)
  [u"hours"]=>
  int(14)
  [u"mday"]=>
  int(31)
  [u"wday"]=>
  int(3)
  [u"mon"]=>
  int(12)
  [u"year"]=>
  int(1969)
  [u"yday"]=>
  int(364)
  [u"weekday"]=>
  unicode(9) "Wednesday"
  [u"month"]=>
  unicode(8) "December"
  [0]=>
  int(0)
}

--Africa/Casablanca--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(0)
  [u"hours"]=>
  int(0)
  [u"mday"]=>
  int(1)
  [u"wday"]=>
  int(4)
  [u"mon"]=>
  int(1)
  [u"year"]=>
  int(1970)
  [u"yday"]=>
  int(0)
  [u"weekday"]=>
  unicode(8) "Thursday"
  [u"month"]=>
  unicode(7) "January"
  [0]=>
  int(0)
}

--Europe/Moscow--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(0)
  [u"hours"]=>
  int(3)
  [u"mday"]=>
  int(1)
  [u"wday"]=>
  int(4)
  [u"mon"]=>
  int(1)
  [u"year"]=>
  int(1970)
  [u"yday"]=>
  int(0)
  [u"weekday"]=>
  unicode(8) "Thursday"
  [u"month"]=>
  unicode(7) "January"
  [0]=>
  int(0)
}

--Asia/Hong_Kong--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(0)
  [u"hours"]=>
  int(8)
  [u"mday"]=>
  int(1)
  [u"wday"]=>
  int(4)
  [u"mon"]=>
  int(1)
  [u"year"]=>
  int(1970)
  [u"yday"]=>
  int(0)
  [u"weekday"]=>
  unicode(8) "Thursday"
  [u"month"]=>
  unicode(7) "January"
  [0]=>
  int(0)
}

--Australia/Brisbane--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(0)
  [u"hours"]=>
  int(10)
  [u"mday"]=>
  int(1)
  [u"wday"]=>
  int(4)
  [u"mon"]=>
  int(1)
  [u"year"]=>
  int(1970)
  [u"yday"]=>
  int(0)
  [u"weekday"]=>
  unicode(8) "Thursday"
  [u"month"]=>
  unicode(7) "January"
  [0]=>
  int(0)
}

--Pacific/Wallis--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(0)
  [u"hours"]=>
  int(12)
  [u"mday"]=>
  int(1)
  [u"wday"]=>
  int(4)
  [u"mon"]=>
  int(1)
  [u"year"]=>
  int(1970)
  [u"yday"]=>
  int(0)
  [u"weekday"]=>
  unicode(8) "Thursday"
  [u"month"]=>
  unicode(7) "January"
  [0]=>
  int(0)
}
===DONE===
