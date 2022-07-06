--TEST--
Basic array unpacking
--FILE--
<?php
$array = [1, 2, 3];

function getArr() {
    return [4, 5];
}

function arrGen() {
    for($i = 11; $i < 15; $i++) {
        yield $i;
    }
}

var_dump([...[]]);
var_dump([...[1, 2, 3]]);
var_dump([...$array]);
var_dump([...getArr()]);
var_dump([...arrGen()]);
var_dump([...new ArrayIterator(['a', 'b', 'c'])]);

var_dump([0, ...$array, ...getArr(), 6, 7, 8, 9, 10, ...arrGen()]);
var_dump([0, ...$array, ...$array, 'end']);

?>
--EXPECT--
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(4) {
  [0]=>
  int(11)
  [1]=>
  int(12)
  [2]=>
  int(13)
  [3]=>
  int(14)
}
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(15) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
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
  [10]=>
  int(10)
  [11]=>
  int(11)
  [12]=>
  int(12)
  [13]=>
  int(13)
  [14]=>
  int(14)
}
array(8) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(1)
  [5]=>
  int(2)
  [6]=>
  int(3)
  [7]=>
  string(3) "end"
}
