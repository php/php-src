--TEST--
Test get_html_translation_table() function : basic functionality - table as HTML_SPECIALCHARS
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style [, string charset_hint]]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

/* test get_html_translation_table() when $table argument is specified as HTML_SPECIALCHARS */

echo "*** Testing get_html_translation_table() : basic functionality ***\n";

// $table as HTML_SEPCIALCHARS and different quote style
echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_COMPAT --\n";
$table = HTML_SPECIALCHARS;
$quote_style = ENT_COMPAT;
var_dump( get_html_translation_table($table, $quote_style, "UTF-8") );

echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_QUOTES --\n";
$quote_style = ENT_QUOTES;
var_dump( get_html_translation_table($table, $quote_style, "UTF-8") );

echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_NOQUOTES --\n";
$quote_style = ENT_NOQUOTES;
var_dump( get_html_translation_table($table, $quote_style, "UTF-8") );

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : basic functionality ***
-- with table = HTML_SPECIALCHARS & quote_style = ENT_COMPAT --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
}
-- with table = HTML_SPECIALCHARS & quote_style = ENT_QUOTES --
array(5) {
  ["&"]=>
  string(5) "&amp;"
  ["""]=>
  string(6) "&quot;"
  ["'"]=>
  string(6) "&#039;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
}
-- with table = HTML_SPECIALCHARS & quote_style = ENT_NOQUOTES --
array(3) {
  ["&"]=>
  string(5) "&amp;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
}
Done
