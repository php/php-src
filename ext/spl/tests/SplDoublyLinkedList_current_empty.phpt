--TEST--
Run current() function on an empty SplDoublyLinkedList.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php

$list = new SplDoublyLinkedList();
var_dump($list->current());

?>
--EXPECT--
NULL
