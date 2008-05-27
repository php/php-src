--TEST--
Test strrev() function : basic functionality 
--FILE--
<?php
/* Prototype  : string strrev(string $str);
 * Description: Reverse a string 
 * Source code: ext/standard/string.c
*/

echo "*** Testing strrev() : basic functionality ***\n";
$heredoc = <<<EOD
Hello, world
EOD;

//regular string
var_dump( strrev("Hello, World") );
var_dump( strrev('Hello, World') );

//single character
var_dump( strrev("H") );
var_dump( strrev('H') );

//string containing simalr chars
var_dump( strrev("HHHHHH") );
var_dump( strrev("HhhhhH") );

//string containing escape char
var_dump( strrev("Hello, World\n") );
var_dump( strrev('Hello, World\n') );

//heredoc string
var_dump( strrev($heredoc) );
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrev() : basic functionality ***
unicode(12) "dlroW ,olleH"
unicode(12) "dlroW ,olleH"
unicode(1) "H"
unicode(1) "H"
unicode(6) "HHHHHH"
unicode(6) "HhhhhH"
unicode(13) "
dlroW ,olleH"
unicode(14) "n\dlroW ,olleH"
unicode(12) "dlrow ,olleH"
*** Done ***
