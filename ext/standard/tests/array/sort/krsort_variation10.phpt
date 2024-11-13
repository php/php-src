--TEST--
Test krsort() function : usage variations - sort heredoc strings
--FILE--
<?php
/*
 * testing krsort() by providing array of heredoc strings for $array argument with
 * following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_STRING  - compare items as strings
*/

echo "*** Testing krsort() : usage variations ***\n";

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

echo "\n-- Testing krsort() by supplying heredoc string array, 'flag' value is default --\n";
$temp_array = $array;
var_dump(krsort($temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing krsort() by supplying heredoc string array, 'flag' = SORT_REGULAR --\n";
$temp_array = $array;
var_dump(krsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing krsort() by supplying heredoc string array, 'flag' = SORT_STRING --\n";
$temp_array = $array;
var_dump(krsort($temp_array, SORT_STRING) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing krsort() : usage variations ***

-- Testing krsort() by supplying heredoc string array, 'flag' value is default --
bool(true)
array(3) {
  ["heredoc string	with!@# and 123
Test this!!!"]=>
  string(43) "heredoc string	with!@# and 123
Test this!!!"
  ["Heredoc"]=>
  string(7) "Heredoc"
  ["HEREDOC"]=>
  string(7) "HEREDOC"
}

-- Testing krsort() by supplying heredoc string array, 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  ["heredoc string	with!@# and 123
Test this!!!"]=>
  string(43) "heredoc string	with!@# and 123
Test this!!!"
  ["Heredoc"]=>
  string(7) "Heredoc"
  ["HEREDOC"]=>
  string(7) "HEREDOC"
}

-- Testing krsort() by supplying heredoc string array, 'flag' = SORT_STRING --
bool(true)
array(3) {
  ["heredoc string	with!@# and 123
Test this!!!"]=>
  string(43) "heredoc string	with!@# and 123
Test this!!!"
  ["Heredoc"]=>
  string(7) "Heredoc"
  ["HEREDOC"]=>
  string(7) "HEREDOC"
}
Done
