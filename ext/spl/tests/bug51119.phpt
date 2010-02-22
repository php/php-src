--TEST--
SPL: LimitIterator zero is valid offset
--FILE--
<?php

$array = array('a', 'b', 'c');
$arrayIterator = new ArrayIterator($array);

try {
  $limitIterator = new LimitIterator($arrayIterator, 0);
  foreach ($limitIterator as $item) {
    echo $item . "\n";
  }
} catch (OutOfRangeException $e){
  print $e->getMessage() . "\n";
}

try {
  $limitIterator = new LimitIterator($arrayIterator, -1);
  foreach ($limitIterator as $item) {
    echo $item . "\n";
  }
} catch (OutOfRangeException $e){
  print $e->getMessage() . "\n";
}

?>
===DONE===
--EXPECT--
a
b
c
Parameter offset must be >= 0
===DONE===
