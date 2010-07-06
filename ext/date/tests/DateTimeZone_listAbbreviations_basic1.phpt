--TEST--
Test DateTimeZone::listAbbreviations() function : basic functionality 
--FILE--
<?php
/* Prototype  : array DateTimeZone::listAbbreviations  ( void  )
 * Description: Returns associative array containing dst, offset and the timezone name
 * Source code: ext/date/php_date.c
 * Alias to functions: timezone_abbreviations_list
 */

echo "*** Testing DateTimeZone::listAbbreviations() : basic functionality ***\n";

//Set the default time zone 
date_default_timezone_set("GMT");

$abbr = DateTimeZone::listAbbreviations();

var_dump( gettype($abbr) );
var_dump( count($abbr) );

echo "\n-- Format a sample entry --\n";
var_dump( $abbr["acst"] );	

?>
===DONE===
--EXPECT--
*** Testing DateTimeZone::listAbbreviations() : basic functionality ***
string(5) "array"
int(399)

-- Format a sample entry --
array(4) {
  [0]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(-14400)
    ["timezone_id"]=>
    string(18) "America/Porto_Acre"
  }
  [1]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(-14400)
    ["timezone_id"]=>
    string(16) "America/Eirunepe"
  }
  [2]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(-14400)
    ["timezone_id"]=>
    string(18) "America/Rio_Branco"
  }
  [3]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(-14400)
    ["timezone_id"]=>
    string(11) "Brazil/Acre"
  }
}
===DONE===
