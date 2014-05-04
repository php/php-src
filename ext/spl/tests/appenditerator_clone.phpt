--TEST--
SPL: AppendIterator clone test
--FILE--
<?php
$arrayIterator1 = new ArrayIterator(array(1, 2));
$arrayIterator2 = new ArrayIterator(array(3, 4));
$it1 = new AppendIterator();
$it1->append($arrayIterator1);
$it2 = clone $it1;
$it2->append($arrayIterator2);

$it1->rewind();
$it1->next();
$it2->rewind();

echo "Original: ";
foreach($it1 as $el) 
  echo "$el, ";
echo PHP_EOL;

echo "Clone: ";
foreach($it2 as $el) 
  echo "$el, ";
echo PHP_EOL;
?>
--EXPECT--
Original: 1, 2, 
Clone: 1, 2, 3, 4, 

