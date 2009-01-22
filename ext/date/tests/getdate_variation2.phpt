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
  [u"seconds"]=>
  int(5)
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
  int(5)
}

--octal 010--
array(11) {
  [u"seconds"]=>
  int(8)
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
  int(8)
}

--octal -010--
array(11) {
  [u"seconds"]=>
  int(52)
  [u"minutes"]=>
  int(29)
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
  int(-8)
}
===DONE===
