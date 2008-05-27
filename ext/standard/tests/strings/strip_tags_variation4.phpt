--TEST--
Test strip_tags() function : usage variations - invalid values for 'str' and valid 'allowable_tags'
--INI--
short_open_tag = on
--FILE--
<?php
/* Prototype  : string strip_tags(string $str [, string $allowable_tags])
 * Description: Strips HTML and PHP tags from a string
 * Source code: ext/standard/string.c
*/

/*
 * testing functionality of strip_tags() by giving invalid values for $str and valid values for $allowable_tags argument
*/

echo "*** Testing strip_tags() : usage variations ***\n";

// unexpected values for $string
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

//valid html and php tags
$quotes = "<p><a><?php<html>";

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
--EXPECT--
*** Testing strip_tags() : usage variations ***
-- Iteration 1 --
unicode(32) "hello 		world... strip_tags_test"
-- Iteration 2 --
unicode(34) "hello \t\tworld... strip_tags_test"
-- Iteration 3 --
unicode(0) ""
-- Iteration 4 --
unicode(0) ""
-- Iteration 5 --
unicode(5) "hello"
-- Iteration 6 --
unicode(5) "hello"
-- Iteration 7 --
unicode(9) "HtMl text"
-- Iteration 8 --
unicode(9) "HtMl text"
-- Iteration 9 --
unicode(26) "I am not a valid html text"
-- Iteration 10 --
unicode(26) "I am not a valid html text"
-- Iteration 11 --
unicode(62) "I am a quoted (") string with special chars like $,\!,\@,\%,\&"
-- Iteration 12 --
unicode(64) "I am a quoted (\") string with special chars like \$,\!,\@,\%,\&"
Done
