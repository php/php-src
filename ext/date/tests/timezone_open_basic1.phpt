--TEST--
Test timezone_open() function : basic functionality 
--FILE--
<?php
/* Prototype  : DateTimeZone timezone_open  ( string $timezone  )
 * Description: Returns new DateTimeZone object
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTime::__construct()
 */

echo "*** Testing timezone_open() : basic functionality ***\n";

var_dump( timezone_open("GMT") );
var_dump( timezone_open("Europe/London") );
var_dump( timezone_open("America/Los_Angeles") );

?>
===DONE===
--EXPECTF--
*** Testing timezone_open() : basic functionality ***
object(DateTimeZone)#%d (0) {
}
object(DateTimeZone)#%d (0) {
}
object(DateTimeZone)#%d (0) {
}
===DONE===