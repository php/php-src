--TEST--
Test ksort() function : usage variations - sort heredoc strings
--FILE--
<?php
/* Prototype  : bool ksort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key, maintaining key to data correlation
 * Source code: ext/standard/array.c
*/

/*
 * testing ksort() by providing array of heredoc strings for $array argument with
 * following flag values:
 *  1.flag value as defualt
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_STRING  - compare items as strings
*/

echo "*** Testing ksort() : usage variations ***\n";

// Different heredoc strings to be sorted
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

$array = array (
  $simple_heredoc1 => "Heredoc",
  $simple_heredoc2 => "HEREDOC",
  $multiline_heredoc => "heredoc string\twith!@# and 123\nTest this!!!"
);

echo "\n-- Testing ksort() by supplying heredoc string array, 'flag' value is defualt --\n";
$temp_array = $array;
var_dump(ksort($temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying heredoc string array, 'flag' = SORT_REGULAR --\n";
$temp_array = $array;
var_dump(ksort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying heredoc string array, 'flag' = SORT_STRING --\n";
$temp_array = $array;
var_dump(ksort($temp_array, SORT_STRING) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing ksort() : usage variations ***

-- Testing ksort() by supplying heredoc string array, 'flag' value is defualt --
bool(true)
array(3) {
  ["HEREDOC"]=>
  string(7) "HEREDOC"
  ["Heredoc"]=>
  string(7) "Heredoc"
  ["heredoc string	with!@# and 123
Test this!!!"]=>
  string(43) "heredoc string	with!@# and 123
Test this!!!"
}

-- Testing ksort() by supplying heredoc string array, 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  ["HEREDOC"]=>
  string(7) "HEREDOC"
  ["Heredoc"]=>
  string(7) "Heredoc"
  ["heredoc string	with!@# and 123
Test this!!!"]=>
  string(43) "heredoc string	with!@# and 123
Test this!!!"
}

-- Testing ksort() by supplying heredoc string array, 'flag' = SORT_STRING --
bool(true)
array(3) {
  ["HEREDOC"]=>
  string(7) "HEREDOC"
  ["Heredoc"]=>
  string(7) "Heredoc"
  ["heredoc string	with!@# and 123
Test this!!!"]=>
  string(43) "heredoc string	with!@# and 123
Test this!!!"
}
Done
