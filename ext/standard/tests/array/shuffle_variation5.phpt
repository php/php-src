--TEST--
Test shuffle() function : usage variation - arrays with diff heredoc strings
--FILE--
<?php
/* Prototype  : bool shuffle(array $array_arg)
 * Description: Randomly shuffle the contents of an array
 * Source code: ext/standard/array.c
*/

/*
* Test behaviour of shuffle() when an array of heredoc strings is passed to
* 'array_arg' argument of the function
*/

echo "*** Testing shuffle() : with array containing heredoc strings ***\n";

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

// defining array with values as heredoc strings
$heredoc_array = array(
  $empty_heredoc,
  $heredoc_with_newline,
  $heredoc_with_characters,
  $heredoc_with_newline_and_tabs,
  $heredoc_with_alphanumerics,
  $heredoc_with_embedded_nulls
);

// defining array with keys as heredoc strings
$heredoc_asso_array = array(
  $empty_heredoc => "heredoc1",
  $heredoc_with_newline => "heredoc2",
  $heredoc_with_characters => "heredoc3",
  $heredoc_with_newline_and_tabs => "heredoc3",
  $heredoc_with_alphanumerics => "heredoc4",
  $heredoc_with_embedded_nulls => "heredoc5"
);

// test shuffle() with array containing heredoc strings as values
echo "\n-- with array of heredoc strings --\n";
var_dump( shuffle($heredoc_array) );
echo "\nThe output array is:\n";
var_dump( $heredoc_array );

// test shuffle() with array containing heredoc strings as its keys
echo "\n-- with array having heredoc strings as keys --\n";
var_dump( shuffle($heredoc_asso_array) );
echo "\nThe output array is:\n";
var_dump( $heredoc_asso_array );

echo "Done";
?>
--EXPECTREGEX--
\*\*\* Testing shuffle\(\) : with array containing heredoc strings \*\*\*

-- with array of heredoc strings --
bool\(true\)

The output array is:
array\(6\) {
  \[0\]=>
  string\([0-9]*\) "[0-9 a-z \n \0 \t]*"
  \[1\]=>
  string\([0-9]*\) "[0-9 a-z \n \0 \t]*"
  \[2\]=>
  string\([0-9]*\) "[0-9 a-z \n \0 \t]*"
  \[3\]=>
  string\([0-9]*\) "[0-9 a-z \n \0 \t]*"
  \[4\]=>
  string\([0-9]*\) "[0-9 a-z \n \0 \t]*"
  \[5\]=>
  string\([0-9]*\) "[0-9 a-z \n \0 \t]*"
}

-- with array having heredoc strings as keys --
bool\(true\)

The output array is:
array\(6\) {
  \[0\]=>
  string\(8\) "[heredoc 1-5]*"
  \[1\]=>
  string\(8\) "[heredoc 1-5]*"
  \[2\]=>
  string\(8\) "[heredoc 1-5]*"
  \[3\]=>
  string\(8\) "[heredoc 1-5]*"
  \[4\]=>
  string\(8\) "[heredoc 1-5]*"
  \[5\]=>
  string\(8\) "[heredoc 1-5]*"
}
Done
