--TEST--
Test get_html_translation_table() function : basic functionality - table as HTML_SPECIALCHARS
--FILE--
<?php
/* test get_html_translation_table() when $table argument is specified as HTML_SPECIALCHARS */

echo "*** Testing get_html_translation_table() : basic functionality ***\n";

// $table as HTML_SEPCIALCHARS and different quote style
echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_COMPAT --\n";
$table = HTML_SPECIALCHARS;
$quote_style = ENT_COMPAT;
$tt = get_html_translation_table($table, $quote_style, "UTF-8");
asort( $tt );
var_dump( $tt );

echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_QUOTES --\n";
$quote_style = ENT_QUOTES;
$tt = get_html_translation_table($table, $quote_style, "UTF-8");
asort( $tt );
var_dump( $tt );

echo "-- with table = HTML_SPECIALCHARS & quote_style = ENT_NOQUOTES --\n";
$quote_style = ENT_NOQUOTES;
$tt = get_html_translation_table($table, $quote_style, "UTF-8");
asort( $tt );
var_dump( $tt );

echo "Done\n";
?>
--EXPECT--
*** Testing get_html_translation_table() : basic functionality ***
-- with table = HTML_SPECIALCHARS & quote_style = ENT_COMPAT --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- with table = HTML_SPECIALCHARS & quote_style = ENT_QUOTES --
array(5) {
  ["'"]=>
  string(6) "&#039;"
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- with table = HTML_SPECIALCHARS & quote_style = ENT_NOQUOTES --
array(3) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
}
Done
