--TEST--
Test array_rand() function : usage variation - with heredoc string as key in the 'input' array
--FILE--
<?php
/*
* Test behaviour of array_rand() when keys of the 'input' array is heredoc string
*/

echo "*** Testing array_rand() : with keys of input array as heredoc strings ***\n";

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

$input = array(
  $empty_heredoc => "heredoc1",
  $heredoc_with_newline => "heredoc2",
  $heredoc_with_characters => "heredoc3",
  $heredoc_with_newline_and_tabs => "heredoc3",
  $heredoc_with_alphanumerics => "heredoc4",
  $heredoc_with_embedded_nulls => "heredoc5"
);

// Test array_rand() function with different valid 'req_num' values
echo "\n-- with default parameters --\n";
var_dump( array_rand($input) );

echo "\n-- with num_req = 1 --\n";
var_dump( array_rand($input, 1) );

echo "\n-- with num_req = 3 --\n";
var_dump( array_rand($input, 3) );

echo "\n-- with num_req = 6 --\n";
var_dump( array_rand($input, 6) );


echo "Done";
?>
--EXPECTREGEX--
\*\*\* Testing array_rand\(\) : with keys of input array as heredoc strings \*\*\*

-- with default parameters --
string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"

-- with num_req = 1 --
string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"

-- with num_req = 3 --
array\(3\) {
  \[0\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
  \[1\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
  \[2\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
}

-- with num_req = 6 --
array\(6\) {
  \[0\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
  \[1\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
  \[2\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
  \[3\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
  \[4\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
  \[5\]=>
  string\([0-9]*\) "[a-z \n \t \0 0-9  ]*"
}
Done
