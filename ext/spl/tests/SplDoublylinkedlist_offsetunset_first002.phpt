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
echo $list->bottom() . "\n";
$list->offsetUnset(0);
echo $list->bottom() . "\n";
?>
--EXPECT--
oh
hai
