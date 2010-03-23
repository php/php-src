--TEST--
Test get_html_translation_table() function : basic functionality - table as HTML_SPECIALCHARS
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == "WIN"){  
  die('skip Not for Windows');
}

if( !setlocale(LC_ALL, "en_US.UTF-8") ) {
  die('skip failed to set locale settings to "en-US.UTF-8"');
}
?>
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

/* test get_html_translation_table() when $table argument is specified as HTML_SPECIALCHARS */

//set locale to en_US.UTF-8
setlocale(LC_ALL, "en_US.UTF-8");

echo "*** Testing get_html_translation_table() : basic functionality ***\n";

// $table as HTML_SEPCIALCHARS and different quote style
echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_COMPAT --\n";
$table = HTML_SPECIALCHARS;
$quote_style = ENT_COMPAT;
var_dump( get_html_translation_table($table, $quote_style) );

echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_QUOTE --\n";
$quote_style = ENT_QUOTES;
var_dump( get_html_translation_table($table, $quote_style) );

echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_NOQUOTE --\n";
$quote_style = ENT_NOQUOTES;
var_dump( get_html_translation_table($table, $quote_style) );

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : basic functionality ***
-- with table = HTML_SPECIALCHARS & quote_style = ENT_COMPAT --
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- with table = HTML_SPECIALCHARS & quote_style = ENT_QUOTE --
array(5) {
  ["""]=>
  string(6) "&quot;"
  ["'"]=>
  string(5) "&#39;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- with table = HTML_SPECIALCHARS & quote_style = ENT_NOQUOTE --
array(3) {
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
Done
