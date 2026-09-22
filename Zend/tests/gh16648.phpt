--TEST--
GH-16648: Use-after-free during array sorting
--FILE--
<?php

function resize_arr() {
    global $arr;
    for ($i = 0; $i < 10; $i++) {
        $arr[$i] = $i;
    }
}

class C {
    function __toString() {
        resize_arr();
        return '3';
    }
}

$arr = ['a' => '1', '3' => new C, '2' => '2'];
asort($arr);
var_dump($arr);

?>
--EXPECT--
array(11) {
  ["a"]=>
  string(1) "1"
  [3]=>
  int(3)
  [2]=>
  int(2)
  [0]=>
  int(0)
  [1]=>
  int(1)
  [4]=>
  int(4)
  [5]=>
  int(5)
  [6]=>
  int(6)
  [7]=>
  int(7)
  [8]=>
  int(8)
  [9]=>
  int(9)
}
