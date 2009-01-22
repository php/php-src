--TEST--
Test getdate() function : usage variation - Verifyig by supplying year-wise sample time stamps since Unix epoch time  
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

	//Year wise time stamps
	'01 Jan 1970' => 0,
	'01 Jan 1971' => 31536000,
	'01 Jan 1972' => 63072000,
	'01 Jan 1973' => 94694400,
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

--01 Jan 1970--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(30)
  [u"hours"]=>
  int(5)
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

--01 Jan 1971--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(30)
  [u"hours"]=>
  int(5)
  [u"mday"]=>
  int(1)
  [u"wday"]=>
  int(5)
  [u"mon"]=>
  int(1)
  [u"year"]=>
  int(1971)
  [u"yday"]=>
  int(0)
  [u"weekday"]=>
  unicode(6) "Friday"
  [u"month"]=>
  unicode(7) "January"
  [0]=>
  int(31536000)
}

--01 Jan 1972--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(30)
  [u"hours"]=>
  int(5)
  [u"mday"]=>
  int(1)
  [u"wday"]=>
  int(6)
  [u"mon"]=>
  int(1)
  [u"year"]=>
  int(1972)
  [u"yday"]=>
  int(0)
  [u"weekday"]=>
  unicode(8) "Saturday"
  [u"month"]=>
  unicode(7) "January"
  [0]=>
  int(63072000)
}

--01 Jan 1973--
array(11) {
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(30)
  [u"hours"]=>
  int(5)
  [u"mday"]=>
  int(1)
  [u"wday"]=>
  int(1)
  [u"mon"]=>
  int(1)
  [u"year"]=>
  int(1973)
  [u"yday"]=>
  int(0)
  [u"weekday"]=>
  unicode(6) "Monday"
  [u"month"]=>
  unicode(7) "January"
  [0]=>
  int(94694400)
}
===DONE===
