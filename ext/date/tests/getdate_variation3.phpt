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
--EXPECTF--
*** Testing getdate() : usage variation ***

--hexadcimal 0x5--
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

--hexadcimal 0xCAFE--
array(11) {
  [u"seconds"]=>
  int(6)
  [u"minutes"]=>
  int(56)
  [u"hours"]=>
  int(19)
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
  int(51966)
}

--octal -0xCAFE--
array(11) {
  [u"seconds"]=>
  int(54)
  [u"minutes"]=>
  int(3)
  [u"hours"]=>
  int(15)
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
  int(-51966)
}
===DONE===
