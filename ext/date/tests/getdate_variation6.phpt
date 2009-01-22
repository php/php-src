--TEST--
Test getdate() function : usage variation - Passing strings containing numbers  
--FILE--
<?php
/* Prototype  : array getdate([int timestamp])
 * Description: Get date/time information 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing getdate() : usage variation ***\n";

date_default_timezone_set("Asia/Calcutta");

//Timezones with required data for date_sunrise
$inputs = array (
		'String 0' => '0',
		'String 10.5' => "10.5",
		'String -10.5' => '-10.5',
);

// loop through each element of the array for timestamp
foreach($inputs as $key => $value) {
      echo "\n--$key--\n";
      var_dump( getdate($value) );
};
?>
===DONE===
--EXPECTF--
*** Testing getdate() : usage variation ***

--String 0--
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

--String 10.5--
array(11) {
  [u"seconds"]=>
  int(10)
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
  int(10)
}

--String -10.5--
array(11) {
  [u"seconds"]=>
  int(50)
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
  int(-10)
}
===DONE===
