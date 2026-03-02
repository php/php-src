--TEST--
Type inference of SEND_UNPACK with empty array
--EXTENSIONS--
opcache
--FILE--
<?php
function test() {
    $array = [1, 2, 3];
    $values = [];
    var_dump(array_push($array, 4, ...$values));
    var_dump($array);
}
test();
?>
--EXPECT--
int(4)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
