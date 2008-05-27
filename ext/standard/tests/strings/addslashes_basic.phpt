--TEST--
Test addslashes() function : basic functionality 
--INI--
--FILE--
<?php
/* Prototype  : string addslashes ( string $str )
 * Description: Returns a string with backslashes before characters (single quotes, double quote, 
 *              backslash and nul character) that need to be quoted in database queries etc.
 * Source code: ext/standard/string.c
*/

/*
 * Testing addslashes() with strings containing characters that can be prefixed with backslash 
 * by the function
*/

echo "*** Testing addslashes() : basic functionality ***\n";

// Initialize all required variables
$str_array = array( "How's everybody",   // string containing single quote
                    'Are you "JOHN"?',   // string with double quotes
                    'c:\php\addslashes',   // string with backslashes
                    "hello\0world"   // string with nul character
                  );

// Calling addslashes() with all arguments
foreach( $str_array as $str )  {
  var_dump( addslashes($str) );
}

echo "Done\n";
?>
--EXPECT--
*** Testing addslashes() : basic functionality ***
unicode(16) "How\'s everybody"
unicode(17) "Are you \"JOHN\"?"
unicode(19) "c:\\php\\addslashes"
unicode(12) "hello\0world"
Done