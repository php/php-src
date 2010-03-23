--TEST--
Bug #44925 (preg_grep() modifies input array)
--FILE--
<?php
$str1 = 'a';
$str2 = 'b';

$array=Array("1",2,3,1.1,FALSE,NULL,Array(), $str1, &$str2);

var_dump($array);

var_dump(preg_grep('/do not match/',$array));

$a = preg_grep('/./',$array);
var_dump($a);

$str1 = 'x';
$str2 = 'y';

var_dump($a); // check if array is still ok

var_dump($array);

?>
--EXPECTF--
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  float(1.1)
  [4]=>
  bool(false)
  [5]=>
  NULL
  [6]=>
  array(0) {
  }
  [7]=>
  string(1) "a"
  [8]=>
  &string(1) "b"
}

Notice: Array to string conversion in %sbug44925.php on line 9
array(0) {
}

Notice: Array to string conversion in %sbug44925.php on line 11
array(7) {
  [0]=>
  string(1) "1"
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  float(1.1)
  [6]=>
  array(0) {
  }
  [7]=>
  string(1) "a"
  [8]=>
  &string(1) "b"
}
array(7) {
  [0]=>
  string(1) "1"
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  float(1.1)
  [6]=>
  array(0) {
  }
  [7]=>
  string(1) "a"
  [8]=>
  &string(1) "y"
}
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  float(1.1)
  [4]=>
  bool(false)
  [5]=>
  NULL
  [6]=>
  array(0) {
  }
  [7]=>
  string(1) "a"
  [8]=>
  &string(1) "y"
}
