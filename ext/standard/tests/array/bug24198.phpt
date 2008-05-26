--TEST--n
Bug #24198 (array_merge_recursive() invalid recursion detection)
--FILE--
<?php
$c = array('a' => 'aa','b' => 'bb'); 

var_dump(array_merge_recursive($c, $c)); 
?>
--EXPECT--
array(2) {
  [u"a"]=>
  array(2) {
    [0]=>
    unicode(2) "aa"
    [1]=>
    unicode(2) "aa"
  }
  [u"b"]=>
  array(2) {
    [0]=>
    unicode(2) "bb"
    [1]=>
    unicode(2) "bb"
  }
}
