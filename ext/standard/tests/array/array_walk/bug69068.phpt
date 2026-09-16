--TEST--
Bug #69068: Exchanging array during array_walk -> memory errors
--FILE--
<?php

$array = [1, 2, 3];
array_walk($array, function($value, $key) {
    var_dump($value);
    if ($value == 2) {
        $GLOBALS['array'] = [4, 5];
    }
});
var_dump($array);

?>
--EXPECT--
int(1)
int(2)
int(4)
int(5)
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
