--TEST--
Test timezone_abbreviations_list() function : basic functionality 
--FILE--
<?php
/* Prototype  : array timezone_abbreviations_list  ( void  )
 * Description: Returns associative array containing dst, offset and the timezone name
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTimeZone::listAbbreviations
 */

echo "*** Testing timezone_abbreviations_list() : basic functionality ***\n";

//Set the default time zone 
date_default_timezone_set("GMT");

$abbr = timezone_abbreviations_list();

var_dump( gettype($abbr) );
var_dump( count($abbr) );

echo "\n-- Format a sample entry --\n";
var_dump( $abbr["acst"] );	

?>
===DONE===
--EXPECTF--
*** Testing timezone_abbreviations_list() : basic functionality ***
string(5) "array"
int(%d)

-- Format a sample entry --
array(11) {
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
    bool(false)
    ["offset"]=>
    int(34200)
    ["timezone_id"]=>
    string(18) "Australia/Adelaide"
  }
  [2]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(-14400)
    ["timezone_id"]=>
    string(16) "America/Eirunepe"
  }
  [3]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(-14400)
    ["timezone_id"]=>
    string(18) "America/Rio_Branco"
  }
  [4]=>
  array(3) {
    ["dst"]=>
    bool(true)
    ["offset"]=>
    int(-14400)
    ["timezone_id"]=>
    string(11) "Brazil/Acre"
  }
  [5]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(34200)
    ["timezone_id"]=>
    string(13) "Asia/Jayapura"
  }
  [6]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(34200)
    ["timezone_id"]=>
    string(21) "Australia/Broken_Hill"
  }
  [7]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(34200)
    ["timezone_id"]=>
    string(16) "Australia/Darwin"
  }
  [8]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(34200)
    ["timezone_id"]=>
    string(15) "Australia/North"
  }
  [9]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(34200)
    ["timezone_id"]=>
    string(15) "Australia/South"
  }
  [10]=>
  array(3) {
    ["dst"]=>
    bool(false)
    ["offset"]=>
    int(34200)
    ["timezone_id"]=>
    string(20) "Australia/Yancowinna"
  }
}
===DONE===
