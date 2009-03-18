--TEST--
Test date_parse() function : basic functionality 
--FILE--
<?php
/* Prototype  : array date_parse  ( string $date  ) 
 * Description: Returns associative array with detailed info about given date.
 * Source code: ext/date/php_date.c
 */
 
//Set the default time zone 
date_default_timezone_set("Europe/London");

echo "*** Testing date_parse() : basic functionality ***\n";

var_dump( date_parse("2009-02-27 10:00:00.5") );
var_dump( date_parse("10:00:00.5") );
var_dump( date_parse("2009-02-27") );

?>
===DONE===
--EXPECT--
*** Testing date_parse() : basic functionality ***
array(12) {
  [u"year"]=>
  int(2009)
  [u"month"]=>
  int(2)
  [u"day"]=>
  int(27)
  [u"hour"]=>
  int(10)
  [u"minute"]=>
  int(0)
  [u"second"]=>
  int(0)
  [u"fraction"]=>
  float(0.5)
  [u"warning_count"]=>
  int(0)
  [u"warnings"]=>
  array(0) {
  }
  [u"error_count"]=>
  int(0)
  [u"errors"]=>
  array(0) {
  }
  [u"is_localtime"]=>
  bool(false)
}
array(12) {
  [u"year"]=>
  bool(false)
  [u"month"]=>
  bool(false)
  [u"day"]=>
  bool(false)
  [u"hour"]=>
  int(10)
  [u"minute"]=>
  int(0)
  [u"second"]=>
  int(0)
  [u"fraction"]=>
  float(0.5)
  [u"warning_count"]=>
  int(0)
  [u"warnings"]=>
  array(0) {
  }
  [u"error_count"]=>
  int(0)
  [u"errors"]=>
  array(0) {
  }
  [u"is_localtime"]=>
  bool(false)
}
array(12) {
  [u"year"]=>
  int(2009)
  [u"month"]=>
  int(2)
  [u"day"]=>
  int(27)
  [u"hour"]=>
  bool(false)
  [u"minute"]=>
  bool(false)
  [u"second"]=>
  bool(false)
  [u"fraction"]=>
  bool(false)
  [u"warning_count"]=>
  int(0)
  [u"warnings"]=>
  array(0) {
  }
  [u"error_count"]=>
  int(0)
  [u"errors"]=>
  array(0) {
  }
  [u"is_localtime"]=>
  bool(false)
}
===DONE===
