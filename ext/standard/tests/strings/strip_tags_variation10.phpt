--TEST--
Test strip_tags() function : usage variations - single quoted strings
--INI--
short_open_tag = on
--FILE--
<?php
/* Prototype  : string strip_tags(string $str [, string $allowable_tags])
 * Description: Strips HTML and PHP tags from a string
 * Source code: ext/standard/string.c
*/

/*
 * testing functionality of strip_tags() by giving single quoted strings as values for $str argument
*/

echo "*** Testing strip_tags() : usage variations ***\n";


$single_quote_string = array (
  '<html> \$ -> This represents the dollar sign</html><?php echo hello ?>',
  '<html>\t\r\v The quick brown fo\fx jumped over the lazy dog</p>',
  '<a>This is a hyper text tag</a>',
  '<? <html>hello world\\t</html>?>',
  '<p>This is a paragraph</p>',
  '<b>This is \ta text in bold letters\r\s\malong with slashes\n</b>'
);

$quotes = "<html><a><p><b><?php";

//loop through the various elements of strings array to test strip_tags() functionality
$iterator = 1;
foreach($single_quote_string as $string_value)
{
      echo "-- Iteration $iterator --\n";
      var_dump( strip_tags($string_value, $quotes) );
      $iterator++;
}

echo "Done";
?>
--EXPECTF--
*** Testing strip_tags() : usage variations ***
-- Iteration 1 --
string(51) "<html> \$ -> This represents the dollar sign</html>"
-- Iteration 2 --
string(63) "<html>\t\r\v The quick brown fo\fx jumped over the lazy dog</p>"
-- Iteration 3 --
string(31) "<a>This is a hyper text tag</a>"
-- Iteration 4 --
string(0) ""
-- Iteration 5 --
string(26) "<p>This is a paragraph</p>"
-- Iteration 6 --
string(65) "<b>This is \ta text in bold letters\r\s\malong with slashes\n</b>"
Done
