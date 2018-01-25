--TEST--
Test get_html_translation_table() function : error conditions
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style [, string charset_hint]]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

echo "*** Testing get_html_translation_table() : error conditions ***\n";

// More than expected number of arguments
echo "\n-- Testing get_html_translation_table() function with more than expected no. of arguments --\n";
$table = HTML_ENTITIES;
$quote_style = ENT_COMPAT;
$extra_arg = 10;

var_dump( get_html_translation_table($table, $quote_style, "UTF-8", $extra_arg) );

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : error conditions ***

-- Testing get_html_translation_table() function with more than expected no. of arguments --

Warning: get_html_translation_table() expects at most 3 parameters, 4 given in %s on line %d
NULL
Done
