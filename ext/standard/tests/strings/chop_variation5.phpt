--TEST--
Test chop() function : usage variations - miscellaneous arguments
--FILE--
<?php
/* Prototype  : string chop ( string $str [, string $charlist] )
 * Description: Strip whitespace (or other characters) from the end of a string
 * Source code: ext/standard/string.c
*/

/*
 * Testing chop() : with miscellaneous arguments
*/

echo "*** Testing chop() : with miscellaneous arguments ***\n";

 var_dump ( chop("chop test   \t\0 ") );                       /* without second Argument */
 var_dump ( chop("chop test   " , "") );                       /* no characters in second Argument */
 var_dump ( chop("chop test        ", NULL) );                 /* with NULL as second Argument */
 var_dump ( chop("chop test        ", true) );                 /* with boolean value as second Argument */
 var_dump ( chop("chop test        ", " ") );                  /* with single space as second Argument */
 var_dump ( chop("chop test \t\n\r\0\x0B", "\t\n\r\0\x0B") );  /* with multiple escape sequences as second Argument */
 var_dump ( chop("chop testABCXYZ", "A..Z") );                 /* with characters range as second Argument */
 var_dump ( chop("chop test0123456789", "0..9") );             /* with numbers range as second Argument */
 var_dump ( chop("chop test$#@", "#@$") );                     /* with some special characters as second Argument */

echo "Done\n";
?>
--EXPECTF--
*** Testing chop() : with miscellaneous arguments ***
string(9) "chop test"
string(12) "chop test   "
string(17) "chop test        "
string(17) "chop test        "
string(9) "chop test"
string(10) "chop test "
string(9) "chop test"
string(9) "chop test"
string(9) "chop test"
Done
