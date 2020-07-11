--TEST--
Test get_html_translation_table() function : basic functionality - XML 1.0
--FILE--
<?php
echo "*** Testing get_html_translation_table() : basic functionality/XML 1.0 ***\n";

echo "-- with table = HTML_ENTITIES, ENT_QUOTES --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_QUOTES | ENT_XML1, "UTF-8");
asort( $tt );
var_dump( count($tt) );
print_r( $tt );

echo "-- with table = HTML_ENTITIES, ENT_COMPAT --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_COMPAT | ENT_XML1, "UTF-8");
var_dump( count($tt) );

echo "-- with table = HTML_ENTITIES, ENT_NOQUOTES --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_NOQUOTES | ENT_XML1, "UTF-8");
var_dump( count($tt) );

echo "-- with table = HTML_SPECIALCHARS, ENT_COMPAT --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_COMPAT, "UTF-8");
asort( $tt );
var_dump( count($tt) );
print_r( $tt );

echo "-- with table = HTML_SPECIALCHARS, ENT_QUOTES --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_QUOTES | ENT_XML1, "UTF-8");
asort( $tt );
var_dump( $tt );

echo "-- with table = HTML_SPECIALCHARS, ENT_NOQUOTES --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_NOQUOTES | ENT_XML1, "UTF-8");
asort( $tt );
var_dump( $tt );


echo "Done\n";
?>
--EXPECT--
*** Testing get_html_translation_table() : basic functionality/XML 1.0 ***
-- with table = HTML_ENTITIES, ENT_QUOTES --
int(5)
Array
(
    [&] => &amp;
    ['] => &apos;
    [>] => &gt;
    [<] => &lt;
    ["] => &quot;
)
-- with table = HTML_ENTITIES, ENT_COMPAT --
int(4)
-- with table = HTML_ENTITIES, ENT_NOQUOTES --
int(3)
-- with table = HTML_SPECIALCHARS, ENT_COMPAT --
int(4)
Array
(
    [&] => &amp;
    [>] => &gt;
    [<] => &lt;
    ["] => &quot;
)
-- with table = HTML_SPECIALCHARS, ENT_QUOTES --
array(5) {
  ["&"]=>
  string(5) "&amp;"
  ["'"]=>
  string(6) "&apos;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- with table = HTML_SPECIALCHARS, ENT_NOQUOTES --
array(3) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
}
Done
