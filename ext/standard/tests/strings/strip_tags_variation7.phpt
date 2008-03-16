--TEST--
Test strip_tags() function : usage variations - invalid values for 'str' and 'allowable_tags'
--INI--
short_open_tag = on
--FILE--
<?php
/* Prototype  : string strip_tags(string $str [, string $allowable_tags])
 * Description: Strips HTML and PHP tags from a string
 * Source code: ext/standard/string.c
*/

/*
 * testing functionality of strip_tags() by giving invalid values for $str and $allowable_tags argument
*/

echo "*** Testing strip_tags() : usage variations ***\n";

$strings = array (
  "<abc>hello</abc> \t\tworld... <ppp>strip_tags_test</ppp>",
  '<abc>hello</abc> \t\tworld... <ppp>strip_tags_test</ppp>',
  "<%?php hello\t world?%>",
  '<%?php hello\t world?%>',
  "<<htmL>>hello<</htmL>>",
  '<<htmL>>hello<</htmL>>',
  "<a.>HtMl text</.a>",
  '<a.>HtMl text</.a>',
  "<nnn>I am not a valid html text</nnn>",
  '<nnn>I am not a valid html text</nnn>',
  "<nnn>I am a quoted (\") string with special chars like \$,\!,\@,\%,\&</nnn>",
  '<nnn>I am a quoted (\") string with special chars like \$,\!,\@,\%,\&</nnn>',
);

$quotes = "<nnn><abc><%?<<html>>";

//loop through the various elements of strings array to test strip_tags() functionality
$iterator = 1;
foreach($strings as $string_value)
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
string(43) "<abc>hello</abc> 		world... strip_tags_test"
-- Iteration 2 --
string(45) "<abc>hello</abc> \t\tworld... strip_tags_test"
-- Iteration 3 --
string(0) ""
-- Iteration 4 --
string(0) ""
-- Iteration 5 --
string(18) "<htmL>hello</htmL>"
-- Iteration 6 --
string(18) "<htmL>hello</htmL>"
-- Iteration 7 --
string(9) "HtMl text"
-- Iteration 8 --
string(9) "HtMl text"
-- Iteration 9 --
string(37) "<nnn>I am not a valid html text</nnn>"
-- Iteration 10 --
string(37) "<nnn>I am not a valid html text</nnn>"
-- Iteration 11 --
string(73) "<nnn>I am a quoted (") string with special chars like $,\!,\@,\%,\&</nnn>"
-- Iteration 12 --
string(75) "<nnn>I am a quoted (\") string with special chars like \$,\!,\@,\%,\&</nnn>"
Done
