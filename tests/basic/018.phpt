--TEST--
POST Method test and arrays - 6 
--POST--
a[][]=1&a[][]=3&b[a][b][c]=1&b[a][b][d]=1
--FILE--
<?php
var_dump($_POST['a']); 
var_dump($_POST['b']); 
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    [0]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    [0]=>
    string(1) "3"
  }
}
array(1) {
  ["a"]=>
  array(1) {
    ["b"]=>
    array(2) {
      ["c"]=>
      string(1) "1"
      ["d"]=>
      string(1) "1"
    }
  }
}
