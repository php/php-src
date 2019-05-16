--TEST--
Test DateTime::format() function : basic functionality
--FILE--
<?php
/* Prototype  : public string DateTime::format  ( string $format  )
 * Description: Returns date formatted according to given format
 * Source code: ext/date/php_date.c
 * Alias to functions: date_format
 */

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::format() : basic functionality ***\n";
$date = new DateTime("2005-07-14 22:30:41");

var_dump( $date->format( "F j, Y, g:i a") );
var_dump( $date->format( "m.d.y") );
var_dump( $date->format( "j, n, Y") );
var_dump( $date->format( "Ymd") );
var_dump( $date->format( 'h-i-s, j-m-y, it is w Day') );
var_dump( $date->format( '\i\t \i\s \t\h\e jS \d\a\y.') );
var_dump( $date->format( "D M j G:i:s T Y") );
var_dump( $date->format( 'H:m:s \m \i\s\ \m\o\n\t\h') );
var_dump( $date->format( "H:i:s") );

?>
===DONE===
--EXPECT--
*** Testing DateTime::format() : basic functionality ***
string(23) "July 14, 2005, 10:30 pm"
string(8) "07.14.05"
string(11) "14, 7, 2005"
string(8) "20050714"
string(39) "10-30-41, 14-07-05, 3031 3041 4 Thupm05"
string(19) "it is the 14th day."
string(28) "Thu Jul 14 22:30:41 BST 2005"
string(19) "22:07:41 m is month"
string(8) "22:30:41"
===DONE===
