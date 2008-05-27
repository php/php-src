--TEST--
Test ltrim() function
--FILE--
<?php

/* Prototype: string ltrim( string str[, string charlist] )
 * Description: Strip whitespace (or other characters) from the beginning of a string
 */

/* Testing for Error conditions */
/*  Invalid Number of Arguments */
var_dump( ltrim() );
var_dump( ltrim("", " ", 1) );

/* heredoc string */
$str = <<<EOD
us
ing heredoc string
EOD;

echo "\n*** Testing with heredoc string ***\n";
var_dump( ltrim($str, "\nusi") );

/* Testing the Normal behaviour of ltrim() function */

echo "\n*** Testing Normal Behaviour ***\n";
var_dump ( ltrim(" \t\0    ltrim test") );  // without second Argument
var_dump ( ltrim("   ltrim test" , "") );  //  no characters in second Argument
var_dump ( ltrim("        ltrim test", NULL) );  // with NULL as second Argument
var_dump ( ltrim("        ltrim test", true) );  // with boolean value as second Argument
var_dump ( ltrim("        ltrim test", " ") );   // with single space as second Argument
var_dump ( ltrim("\t\n\r\0\x0B ltrim test", "\t\n\r\0\x0B") );  // with multiple escape sequences as second Argument
var_dump ( ltrim("ABCXYZltrim test", "A..Z") );  // with characters range as second Argument
var_dump ( ltrim("0123456789ltrim test", "0..9") );  // with numbers range as second Argument
var_dump ( ltrim("@$#ltrim test", "#@$") );  // with some special characters as second Argument

echo "\n*** Testing for  scalar argument ***\n";
var_dump( ltrim(  12345  ) );  // Scalar argument

echo "\n*** Testing for  NULL argument ***\n";
var_dump( ltrim(NULL) );  // NULL Argument

echo "\nDone";

?>
--EXPECTF--
Warning: ltrim() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: ltrim() expects at most 2 parameters, 3 given in %s on line %d
NULL

*** Testing with heredoc string ***
unicode(17) "ng heredoc string"

*** Testing Normal Behaviour ***
unicode(10) "ltrim test"
unicode(13) "   ltrim test"
unicode(18) "        ltrim test"
unicode(18) "        ltrim test"
unicode(10) "ltrim test"
unicode(11) " ltrim test"
unicode(10) "ltrim test"
unicode(10) "ltrim test"
unicode(10) "ltrim test"

*** Testing for  scalar argument ***
unicode(5) "12345"

*** Testing for  NULL argument ***
unicode(0) ""

Done
