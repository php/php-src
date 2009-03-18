--TEST--
Test new DateTime() : basic functionality 
--FILE--
<?php
/* Prototype  : DateTime::__construct  ([ string $time="now"  [, DateTimeZone $timezone=NULL  ]] )
 * Description: Returns new DateTime object
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

//Set the default time zone 
date_default_timezone_set("Europe/London");

echo "*** Testing new DateTime() : basic functionality ***\n";

var_dump( new DateTime('') );

var_dump( new DateTime("GMT") );
var_dump( new DateTime("2005-07-14 22:30:41") );
var_dump( new DateTime("2005-07-14 22:30:41 GMT") );

?>
===DONE===
--EXPECTF--
*** Testing new DateTime() : basic functionality ***
object(DateTime)#%d (3) {
  [u"date"]=>
  unicode(19) "%s"
  [u"timezone_type"]=>
  int(3)
  [u"timezone"]=>
  unicode(13) "Europe/London"
}
object(DateTime)#%d (3) {
  [u"date"]=>
  unicode(19) "%s"
  [u"timezone_type"]=>
  int(2)
  [u"timezone"]=>
  unicode(3) "GMT"
}
object(DateTime)#%d (3) {
  [u"date"]=>
  unicode(19) "2005-07-14 22:30:41"
  [u"timezone_type"]=>
  int(3)
  [u"timezone"]=>
  unicode(13) "Europe/London"
}
object(DateTime)#%d (3) {
  [u"date"]=>
  unicode(19) "2005-07-14 22:30:41"
  [u"timezone_type"]=>
  int(2)
  [u"timezone"]=>
  unicode(3) "GMT"
}
===DONE===
