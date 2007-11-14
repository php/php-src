--TEST--
Test uasort() function : usage variations - sort diff. strings 
--FILE--
<?php
/* Prototype  : bool uasort(array $array_arg, string $cmp_function)
 * Description: Sort an array with a user-defined comparison function and maintain index association 
 * Source code: ext/standard/array.c
*/

/*
* sorting different strings:
*  single quoted, double quoted and heredoc strings
*/

// comparison function
/* Prototype : int cmp_function(mixed $value1, mixed $value2)
 * Parameters : $value1 and $value2 - values to be compared
 * Return value : 0 - if both values are same
 *                1 - if value1 is greater than value2
 *               -1 - if value1 is less than value2
 * Description : compares value1 and value2
 */
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


echo "*** Testing uasort() : different string arrays as 'array_arg' ***\n";

// Single quoted strings
$single_quoted_values = array(
  0 => ' ', 1 => 'test', 3 => 'Hello', 4 => 'HELLO',
  5 => '', 6 => '\t', 7 => '0', 8 => '123Hello', 9 => '\'', 10 => '@#$%'
);
echo "-- Sorting Single Quoted String values --\n";
var_dump( uasort($single_quoted_values, 'cmp_function') );  // expecting: bool(true)
var_dump($single_quoted_values);

// Double quoted strings
$double_quoted_values = array(
  0 => " ", 1 => "test", 3 => "Hello", 4 => "HELLO", 
  5 => "", 6 => "\t", 7 => "0", 8 => "123Hello", 9 => "\"", 10 => "@#$%"
);
echo "-- Sorting Double Quoted String values --\n";
var_dump( uasort($double_quoted_values, 'cmp_function') );  // expecting: bool(true)
var_dump($double_quoted_values);

// Heredoc strings
$heredoc_values = array(0 => $empty_heredoc, 1 => $simple_heredoc1, 2 => $simple_heredoc2, 3 => $multiline_heredoc);
echo "-- Sorting Heredoc String values --\n";
var_dump( uasort($heredoc_values, 'cmp_function') );  // expecting: bool(true)
var_dump($heredoc_values);

echo "Done"
?>
--EXPECTF--
*** Testing uasort() : different string arrays as 'array_arg' ***
-- Sorting Single Quoted String values --
bool(true)
array(10) {
  [5]=>
  string(0) ""
  [0]=>
  string(1) " "
  [9]=>
  string(1) "'"
  [7]=>
  string(1) "0"
  [8]=>
  string(8) "123Hello"
  [10]=>
  string(4) "@#$%"
  [4]=>
  string(5) "HELLO"
  [3]=>
  string(5) "Hello"
  [6]=>
  string(2) "\t"
  [1]=>
  string(4) "test"
}
-- Sorting Double Quoted String values --
bool(true)
array(10) {
  [5]=>
  string(0) ""
  [6]=>
  string(1) "	"
  [0]=>
  string(1) " "
  [9]=>
  string(1) """
  [7]=>
  string(1) "0"
  [8]=>
  string(8) "123Hello"
  [10]=>
  string(4) "@#$%"
  [4]=>
  string(5) "HELLO"
  [3]=>
  string(5) "Hello"
  [1]=>
  string(4) "test"
}
-- Sorting Heredoc String values --
bool(true)
array(4) {
  [0]=>
  string(0) ""
  [2]=>
  string(7) "HEREDOC"
  [1]=>
  string(7) "Heredoc"
  [3]=>
  string(43) "heredoc string	with!@# and 123
Test this!!!"
}
Done
