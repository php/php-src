--TEST--
Spl\ReverseArrayIterator: Iterator interface (not-empty)
--FILE--
<?php

$ht = [1, 3, 5]; 

$it = new Spl\ReverseArrayIterator($ht);

var_dump($it);
foreach ($it as $key => $value) {
    var_export([$key => $value]);
    echo "\n";
}
var_dump($it);

echo "\nagain, manually this time\n\n";

$it->rewind();
var_dump($it);
while ($it->valid()) {
    var_export([$it->key() => $it->current()]);
    echo "\n";
    $it->next();
}
var_dump($it);

?>
--EXPECTF--
object(Spl\ReverseArrayIterator)#%d (2) {
  ["inner"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(3)
    [2]=>
    int(5)
  }
  ["offset"]=>
  int(2)
}
array (
  2 => 5,
)
array (
  1 => 3,
)
array (
  0 => 1,
)
object(Spl\ReverseArrayIterator)#%d (2) {
  ["inner"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(3)
    [2]=>
    int(5)
  }
  ["offset"]=>
  int(3)
}

again, manually this time

object(Spl\ReverseArrayIterator)#%d (2) {
  ["inner"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(3)
    [2]=>
    int(5)
  }
  ["offset"]=>
  int(2)
}
array (
  2 => 5,
)
array (
  1 => 3,
)
array (
  0 => 1,
)
object(Spl\ReverseArrayIterator)#%d (2) {
  ["inner"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(3)
    [2]=>
    int(5)
  }
  ["offset"]=>
  int(3)
}
