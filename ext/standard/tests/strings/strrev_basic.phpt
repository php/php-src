--TEST--
Test strrev() function : basic functionality
--FILE--
<?php
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
string(12) "dlroW ,olleH"
string(12) "dlroW ,olleH"
string(1) "H"
string(1) "H"
string(6) "HHHHHH"
string(6) "HhhhhH"
string(13) "
dlroW ,olleH"
string(14) "n\dlroW ,olleH"
string(12) "dlrow ,olleH"
*** Done ***
