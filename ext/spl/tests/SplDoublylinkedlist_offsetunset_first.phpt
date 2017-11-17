--TEST--
SPL: SplDoublyLinkedList : offsetUnset - first element
--CREDITS--
PHPNW TestFest2009 - Rowan Merewood <rowan@merewood.org>
--FILE--
<?php
$list = new SplDoublyLinkedList();
$list->push('oh');
$list->push('hai');
$list->push('thar');
$list->offsetUnset(0);
var_dump($list);
?>
--EXPECTF--
object(SplDoublyLinkedList)#1 (2) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(0)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(2) {
    [0]=>
    %string|unicode%(3) "hai"
    [1]=>
    %string|unicode%(4) "thar"
  }
}
