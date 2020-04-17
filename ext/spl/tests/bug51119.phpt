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
} catch (\ValueError $e){
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
a
b
c
LimitIterator::__construct(): Argument #1 ($iterator) must be greater or equal to 0
