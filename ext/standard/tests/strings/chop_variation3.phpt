--TEST--
Test chop() function : usage variations - with heredoc string
--FILE--
<?php
/*
 * Testing chop() : with heredoc strings
*/

echo "*** Testing chop() : with heredoc strings ***\n";

// defining different heredoc strings
$empty_heredoc = <<<EOT
EOT;

$heredoc_with_newline = <<<EOT
\n

EOT;

$heredoc_with_characters = <<<EOT
first line of heredoc string
second line of heredoc string
third line of heredocstring
EOT;

$heredoc_with_newline_and_tabs = <<<EOT
hello\tworld\nhello\nworld\n
EOT;

$heredoc_with_alphanumerics = <<<EOT
hello123world456
1234hello\t1234
EOT;

$heredoc_with_embedded_nulls = <<<EOT
hello\0world\0hello
\0hello\0
EOT;

$heredoc_strings = array(
                   $empty_heredoc,
                   $heredoc_with_newline,
                   $heredoc_with_characters,
           $heredoc_with_newline_and_tabs,
           $heredoc_with_alphanumerics,
           $heredoc_with_embedded_nulls
           );
$count = 1;
foreach($heredoc_strings as $string)  {
  echo "\n--- Iteration $count ---\n";
  var_dump( chop($string) );
  var_dump( chop($string, "12345o\0\n\t") );
  $count++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing chop() : with heredoc strings ***

--- Iteration 1 ---
string(0) ""
string(0) ""

--- Iteration 2 ---
string(0) ""
string(0) ""

--- Iteration 3 ---
string(86) "first line of heredoc string
second line of heredoc string
third line of heredocstring"
string(86) "first line of heredoc string
second line of heredoc string
third line of heredocstring"

--- Iteration 4 ---
string(23) "hello	world
hello
world"
string(23) "hello	world
hello
world"

--- Iteration 5 ---
string(31) "hello123world456
1234hello	1234"
string(25) "hello123world456
1234hell"

--- Iteration 6 ---
string(24) "hello%0world%0hello
%0hello"
string(23) "hello%0world%0hello
%0hell"
Done
