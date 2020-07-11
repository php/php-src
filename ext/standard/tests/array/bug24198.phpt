--TEST--n
Bug #24198 (array_merge_recursive() invalid recursion detection)
--FILE--
<?php
$c = array('a' => 'aa','b' => 'bb');

var_dump(array_merge_recursive($c, $c));
?>
--EXPECT--
array(2) {
  ["a"]=>
  array(2) {
    [0]=>
    string(2) "aa"
    [1]=>
    string(2) "aa"
  }
  ["b"]=>
  array(2) {
    [0]=>
    string(2) "bb"
    [1]=>
    string(2) "bb"
  }
}
