--TEST--
Collections\Deque popFront
--FILE--
<?php

$dq = new Collections\Deque([new stdClass(), strtoupper('test')]);
var_dump($dq->popFront());
var_dump($dq[0]);
?>
--EXPECT--
object(stdClass)#2 (0) {
}
string(4) "TEST"
