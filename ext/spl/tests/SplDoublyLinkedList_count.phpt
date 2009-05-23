--TEST--
Check that SplDoublyLinkedList::count fails if parameter passed in
--CREDITS--
Rob Knight <themanhimself@robknight.org.uk> PHPNW Test Fest 2009
--FILE--
<?php
$list = new SplDoublyLinkedList();

$c = $list->count('foo');
?>
--EXPECTF--
Warning: SplDoublyLinkedList::count() expects exactly 0 parameters, 1 given in %s on line 4
