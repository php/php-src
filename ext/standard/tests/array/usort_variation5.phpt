--TEST--
Test usort() function : usage variations - string data
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, Unicode string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function 
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays of string data to usort() to test how it is re-ordered
 */

echo "*** Testing usort() : usage variation ***\n";

function cmp_function($value1, $value2)
{
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 > $value2) {
    return 1;
  }
  else {
    return -1;
  }
}

// Different heredoc strings to be sorted 
$empty_heredoc =<<<EOT
EOT;

$simple_heredoc1 =<<<EOT
Heredoc
EOT;

$simple_heredoc2 =<<<EOT
HEREDOC
EOT;

$multiline_heredoc =<<<EOT
heredoc string\twith!@# and 123
Test this!!!
EOT;

// Single quoted strings
$single_quoted_values = array(
  0 => ' ',  1 => 'test', 3 => 'Hello', 4 => 'HELLO',
  5 => '',   6 => '\t',   7 => '0',     8 => '123Hello', 
  9 => '\'', 10 => '@#$%'
);

echo "\n-- Sorting Single Quoted String values --\n";
var_dump( usort($single_quoted_values, 'cmp_function') );
var_dump($single_quoted_values);

// Double quoted strings
$double_quoted_values = array(
  0 => " ",  1 => "test", 3 => "Hello", 4 => "HELLO", 
  5 => "",   6 => "\t",   7 => "0",     8 => "123Hello", 
  9 => "\"", 10 => "@#$%"
);

echo "\n-- Sorting Double Quoted String values --\n";
var_dump( usort($double_quoted_values, 'cmp_function') );
var_dump($double_quoted_values);

// Heredoc strings
$heredoc_values = array(0 => $empty_heredoc,   1 => $simple_heredoc1, 
                        2 => $simple_heredoc2, 3 => $multiline_heredoc);

echo "\n-- Sorting Heredoc String values --\n";
var_dump( usort($heredoc_values, 'cmp_function') );
var_dump($heredoc_values);
?>
===DONE===
--EXPECTF--
*** Testing usort() : usage variation ***

-- Sorting Single Quoted String values --
bool(true)
array(10) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(1) " "
  [2]=>
  unicode(1) "'"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(8) "123Hello"
  [5]=>
  unicode(4) "@#$%"
  [6]=>
  unicode(5) "HELLO"
  [7]=>
  unicode(5) "Hello"
  [8]=>
  unicode(2) "\t"
  [9]=>
  unicode(4) "test"
}

-- Sorting Double Quoted String values --
bool(true)
array(10) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(1) "	"
  [2]=>
  unicode(1) " "
  [3]=>
  unicode(1) """
  [4]=>
  unicode(1) "0"
  [5]=>
  unicode(8) "123Hello"
  [6]=>
  unicode(4) "@#$%"
  [7]=>
  unicode(5) "HELLO"
  [8]=>
  unicode(5) "Hello"
  [9]=>
  unicode(4) "test"
}

-- Sorting Heredoc String values --
bool(true)
array(4) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(7) "HEREDOC"
  [2]=>
  unicode(7) "Heredoc"
  [3]=>
  unicode(%d) "heredoc string	with!@# and 123
Test this!!!"
}
===DONE===