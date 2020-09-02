--TEST--
Test date_format() function : basic functionality
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing date_format() : basic functionality - formatting coinstants ***\n";
$date = new DateTime("2005-07-14 22:30:41");

var_dump( $date->format( DateTime::ATOM) ) ;
var_dump( $date->format( DateTime::COOKIE) ) ;
var_dump( $date->format( DateTime::ISO8601) ) ;
var_dump( $date->format( DateTime::RFC822) ) ;
var_dump( $date->format( DateTime::RFC850) ) ;
var_dump( $date->format( DateTime::RFC1036) ) ;
var_dump( $date->format( DateTime::RFC1123) ) ;
var_dump( $date->format( DateTime:: RFC2822) ) ;
var_dump( $date->format( DateTime::RFC3339) ) ;
var_dump( $date->format( DateTime::RSS) ) ;
var_dump( $date->format( DateTime::W3C) ) ;

?>
--EXPECT--
*** Testing date_format() : basic functionality - formatting coinstants ***
string(25) "2005-07-14T22:30:41+01:00"
string(34) "Thursday, 14-Jul-2005 22:30:41 BST"
string(24) "2005-07-14T22:30:41+0100"
string(29) "Thu, 14 Jul 05 22:30:41 +0100"
string(32) "Thursday, 14-Jul-05 22:30:41 BST"
string(29) "Thu, 14 Jul 05 22:30:41 +0100"
string(31) "Thu, 14 Jul 2005 22:30:41 +0100"
string(31) "Thu, 14 Jul 2005 22:30:41 +0100"
string(25) "2005-07-14T22:30:41+01:00"
string(31) "Thu, 14 Jul 2005 22:30:41 +0100"
string(25) "2005-07-14T22:30:41+01:00"
