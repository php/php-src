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
  ["year"]=>
  int(2009)
  ["month"]=>
  int(2)
  ["day"]=>
  int(27)
  ["hour"]=>
  int(10)
  ["minute"]=>
  int(0)
  ["second"]=>
  int(0)
  ["fraction"]=>
  float(0.5)
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}
array(12) {
  ["year"]=>
  bool(false)
  ["month"]=>
  bool(false)
  ["day"]=>
  bool(false)
  ["hour"]=>
  int(10)
  ["minute"]=>
  int(0)
  ["second"]=>
  int(0)
  ["fraction"]=>
  float(0.5)
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}
array(12) {
  ["year"]=>
  int(2009)
  ["month"]=>
  int(2)
  ["day"]=>
  int(27)
  ["hour"]=>
  bool(false)
  ["minute"]=>
  bool(false)
  ["second"]=>
  bool(false)
  ["fraction"]=>
  bool(false)
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}
===DONE===
