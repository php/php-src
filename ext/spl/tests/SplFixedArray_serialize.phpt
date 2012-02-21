--TEST--
SplFixedArray serialisation
--FILE--
<?php

$array = new SplFixedArray(5);

$obj = new stdClass;
$obj->prop = 'value';

$array[0] = 'foo';
$array[2] = 42;
$array[3] = $obj;
$array[4] = range(1, 5);

$ser = serialize($array);
echo "$ser\n";
$unser = unserialize($ser);

printf("count: %d\n", count($unser));
printf("getSize(): %d\n", $unser->getSize());

var_dump($unser[0], $unser[1], $unser[2], $unser[3], $unser[4]);

$unser[4] = 'quux';
var_dump($unser[4]);

?>
--EXPECT--
O:13:"SplFixedArray":5:{i:0;s:3:"foo";i:1;N;i:2;i:42;i:3;O:8:"stdClass":1:{s:4:"prop";s:5:"value";}i:4;a:5:{i:0;i:1;i:1;i:2;i:2;i:3;i:3;i:4;i:4;i:5;}}
count: 5
getSize(): 5
string(3) "foo"
NULL
int(42)
object(stdClass)#4 (1) {
  ["prop"]=>
  string(5) "value"
}
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
string(4) "quux"
