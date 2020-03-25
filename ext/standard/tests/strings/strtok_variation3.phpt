--TEST--
Test strtok() function : usage variations - with heredoc strings
--FILE--
<?php
/* Prototype  : string strtok ( str $str, str $token )
 * Description: splits a string (str) into smaller strings (tokens), with each token being delimited by any character from token
 * Source code: ext/standard/string.c
*/

/*
 * Testing strtok() : with heredoc strings
*/

echo "*** Testing strtok() : with heredoc strings ***\n";

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

// loop through each element of the array and check the working of strtok()
// when supplied with different string values

$count = 1;
foreach($heredoc_strings as $string)  {
  echo "\n--- Iteration $count ---\n";
  var_dump( strtok($string, "5o\0\n\t") );
  for($counter = 1; $counter <= 10; $counter++)  {
    var_dump( strtok("5o\0\n\t") );
  }
  $count++;
}


echo "Done\n";
?>
--EXPECT--
*** Testing strtok() : with heredoc strings ***

--- Iteration 1 ---
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 2 ---
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 3 ---
string(11) "first line "
string(7) "f hered"
string(8) "c string"
string(3) "sec"
string(8) "nd line "
string(7) "f hered"
string(8) "c string"
string(11) "third line "
string(7) "f hered"
string(7) "cstring"
bool(false)

--- Iteration 4 ---
string(4) "hell"
string(1) "w"
string(3) "rld"
string(4) "hell"
string(1) "w"
string(3) "rld"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 5 ---
string(4) "hell"
string(4) "123w"
string(4) "rld4"
string(1) "6"
string(8) "1234hell"
string(4) "1234"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 6 ---
string(4) "hell"
string(1) "w"
string(3) "rld"
string(4) "hell"
string(4) "hell"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done
