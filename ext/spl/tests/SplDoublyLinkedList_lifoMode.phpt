--TEST--
Check that SplDoublyLinkedList can traverse backwards
--CREDITS--
Rob Knight <themanhimself@robknight.org.uk> PHPNW Test Fest 2009
--FILE--
<?php
$list = new SplDoublyLinkedList();

$list->push('o');
$list->push('o');
$list->push('f');

$list->setIteratorMode(SplDoublyLinkedList::IT_MODE_LIFO);

$list->rewind();

while ($tmp = $list->current()) {
  echo $tmp;
  $list->next();
}
?>
--EXPECT--
foo