--TEST--
Type of values yielded by SplPriorityQueue to 'foreach' loop do not change partway
--FILE--
<?php

$q = new SplPriorityQueue();
$q->insert('a', 10);
$q->insert('b', 20);
$q->insert('c', 30);

$q->setExtractFlags(SplPriorityQueue::EXTR_DATA);
foreach ($q as $item) {
  $q->setExtractFlags(SplPriorityQueue::EXTR_BOTH);
  echo $item, "\n";
}

?>
--EXPECT--
c
b
a
