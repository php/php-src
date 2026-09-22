--TEST--
Bug #69068: Exchanging array during array_walk -> memory errors (variation)
--FILE--
<?php

$array = [1, 2, 3];
$array2 = [4, 5];
array_walk($array, function(&$value, $key) use ($array2) {
    var_dump($value);
    if ($value == 2) {
        $GLOBALS['array'] = $array2;
    }
    $value *= 10;
});
var_dump($array, $array2);

?>
--EXPECT--
int(1)
int(2)
int(4)
int(5)
array(2) {
  [0]=>
  int(40)
  [1]=>
  int(50)
}
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
