--TEST--
Collections\Deque isEmpty()
--FILE--
<?php
$it = new Collections\Deque();
var_dump($it->isEmpty());
$it->push(123);
var_dump($it->isEmpty());
$it->push('other');
var_dump($it->isEmpty());
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
