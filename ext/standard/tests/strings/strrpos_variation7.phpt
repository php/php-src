--TEST--
Test strrpos() function : usage variations - empty heredoc string for 'haystack' argument
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

/* Test strrpos() function by passing empty heredoc string for haystack
 *  and with various needles & offsets
*/

echo "*** Testing strrpos() function: with heredoc strings ***\n";
echo "-- With empty heredoc string --\n";
$empty_string = <<<EOD
EOD;
var_dump( strrpos($empty_string, "") );
var_dump( strrpos($empty_string, "", 1) );
var_dump( strrpos($empty_string, FALSE) );
var_dump( strrpos($empty_string, NULL) );

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrpos() function: with heredoc strings ***
-- With empty heredoc string --
bool(false)
bool(false)

Deprecated: strrpos(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)

Deprecated: strrpos(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
*** Done ***
