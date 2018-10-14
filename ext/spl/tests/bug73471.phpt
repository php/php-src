--TEST--
Bug #73471 PHP freezes with AppendIterator
--FILE--
<?php

$iterator = new \AppendIterator();
$events = new \ArrayIterator([1,2,3,4,5]);
$events2 = new \ArrayIterator(['a', 'b', 'c']);
$iterator->append($events);
foreach($events as $event){}
$iterator->append($events2);
?>
===DONE===
--EXPECT--
===DONE===
