--TEST--
Test getdate() function : basic functionality 
--FILE--
<?php
/* Prototype  : array getdate([int timestamp])
 * Description: Get date/time information 
 * Source code: ext/date/php_date.c
 */

echo "*** Testing getdate() : basic functionality ***\n";

//Set the default time zone 
date_default_timezone_set("Asia/Calcutta");

// Initialise all required variables
$timestamp = 10;

// Calling getdate() with all possible arguments
var_dump( getdate($timestamp) );

// Calling getdate() with mandatory arguments
var_dump( getdate() );

?>
===DONE===
--EXPECTF--
*** Testing getdate() : basic functionality ***
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
array(11) {
  [u"seconds"]=>
  int(%d)
  [u"minutes"]=>
  int(%d)
  [u"hours"]=>
  int(%d)
  [u"mday"]=>
  int(%d)
  [u"wday"]=>
  int(%d)
  [u"mon"]=>
  int(%d)
  [u"year"]=>
  int(%d)
  [u"yday"]=>
  int(%d)
  [u"weekday"]=>
  unicode(%d) %s
  [u"month"]=>
  unicode(%d) %s
  [0]=>
  int(%d)
}
===DONE===
