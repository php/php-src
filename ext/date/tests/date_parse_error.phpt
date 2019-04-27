--TEST--
Test date_parse() function : error conditions
--FILE--
<?php
/* Prototype  : array date_parse  ( string $date  )
 * Description: Returns associative array with detailed info about given date.
 * Source code: ext/date/php_date.c
 */

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing date_parse() : error conditions ***\n";

echo "\n-- Testing date_parse() function with unexpected characters in \$date argument --\n";
$invalid_date = "2OO9-02--27 10:00?00.5";
var_dump( date_parse($invalid_date) );

?>
===DONE===
--EXPECTF--
*** Testing date_parse() : error conditions ***

-- Testing date_parse() function with unexpected characters in $date argument --
array(13) {
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
  float(0)
  ["warning_count"]=>
  int(1)
  ["warnings"]=>
  array(1) {
    [4]=>
    string(29) "Double timezone specification"
  }
  ["error_count"]=>
  int(7)
  ["errors"]=>
  array(7) {
    [0]=>
    string(20) "Unexpected character"
    [1]=>
    string(47) "The timezone could not be found in the database"
    [3]=>
    string(20) "Unexpected character"
    [7]=>
    string(20) "Unexpected character"
    [8]=>
    string(29) "Double timezone specification"
    [17]=>
    string(20) "Unexpected character"
    [18]=>
    string(25) "Double time specification"
  }
  ["is_localtime"]=>
  bool(true)
  ["zone_type"]=>
  int(0)
}
===DONE===
