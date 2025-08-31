--TEST--
Test array_flip() function : usage variations - 'input' argument with different valid values
--FILE--
<?php
/*
* In 'input' array argument, values are expected to be valid keys i.e. string/integer
* here testing for all different valid string and integer values
*/

echo "*** Testing array_flip() : different valid values in 'input' array argument ***\n";

$empty_heredoc = <<<EOT1
EOT1;

$simple_heredoc = <<<EOT4
simple
EOT4;

$multiline_heredoc = <<<EOT7
multiline heredoc with 123 and
speci@! ch@r$..checking\nwith\talso
EOT7;

$input = array(
  // numeric values
  'int_value' => 1,
  'negative_value' => -2,
  'zero_value' => 0,
  'octal_value' => 012,
  'hex_value' => 0x23,

  // single quoted string value
  'empty_value1' => '',
  'space_value1' => ' ',
  'char_value1' => 'a',
  'string_value1' => 'string1',
  'numeric_value1' => '123',
  'special_char_value1' => '!@#$%',
  'whitespace1_value1' => '\t',
  'whitespace2_value1' => '\n',
  'null_char_value1' => '\0',

  // double quoted string value
  'empty_value2' => "",
  'space_value2' => " ",
  'char_value2' => "b",
  'string_value2' => "string2",
  'numeric_value2' => "456",
  'special_char_value2' => "^&*",
  'whitespace1_value2' => "\t",
  'whitespace2_value2' => "\n",
  'null_char_value2' => "\0",
  'binary_value' => "a".chr(0)."b",

  // heredoc string value
  'empty_heredoc' => $empty_heredoc,
  'simple_heredoc' => $simple_heredoc,
  'multiline_heredoc' => $multiline_heredoc,
);

var_dump( array_flip($input) );

echo "Done"
?>
--EXPECTF--
*** Testing array_flip() : different valid values in 'input' array argument ***
array(24) {
  [1]=>
  string(9) "int_value"
  [-2]=>
  string(14) "negative_value"
  [0]=>
  string(10) "zero_value"
  [10]=>
  string(11) "octal_value"
  [35]=>
  string(9) "hex_value"
  [""]=>
  string(13) "empty_heredoc"
  [" "]=>
  string(12) "space_value2"
  ["a"]=>
  string(11) "char_value1"
  ["string1"]=>
  string(13) "string_value1"
  [123]=>
  string(14) "numeric_value1"
  ["!@#$%"]=>
  string(19) "special_char_value1"
  ["\t"]=>
  string(18) "whitespace1_value1"
  ["\n"]=>
  string(18) "whitespace2_value1"
  ["\0"]=>
  string(16) "null_char_value1"
  ["b"]=>
  string(11) "char_value2"
  ["string2"]=>
  string(13) "string_value2"
  [456]=>
  string(14) "numeric_value2"
  ["^&*"]=>
  string(19) "special_char_value2"
  ["	"]=>
  string(18) "whitespace1_value2"
  ["
"]=>
  string(18) "whitespace2_value2"
  ["%0"]=>
  string(16) "null_char_value2"
  ["a%0b"]=>
  string(12) "binary_value"
  ["simple"]=>
  string(14) "simple_heredoc"
  ["multiline heredoc with 123 and
speci@! ch@r$..checking
with	also"]=>
  string(17) "multiline_heredoc"
}
Done
