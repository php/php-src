--TEST--
SPL: DoublyLinkedList: insert operations
--FILE--
<?php
$dll = new SplDoublyLinkedList();
// errors
try {
	$dll->insertBeforeOffset(1,5);
} catch (OutOfRangeException $e) {
	echo "Exception: ".$e->getMessage()."\n";
}
try {
	$dll->insertBeforeOffset(2,1);
} catch (OutOfRangeException $e) {
	echo "Exception: ".$e->getMessage()."\n";
}

$dll->push(3);						//	3
$dll->push(6);						//	3 6
// Insert in the middle of the DLL
$dll->insertBeforeOffset(1,4);		//	3 4 6
$dll->insertAfterOffset(1,5);		//	3 4 5 6
$dll->unshift(2);					//	2 3 4 5 6
// Insert at the beginning and end of the DLL
$dll->insertBeforeOffset(0,1);		//	1 2 3 4 5 6
$dll->insertAfterOffset(5,7);		//	1 2 3 4 5 6 7

echo count($dll)."\n";

echo $dll->pop()."\n";
echo $dll->pop()."\n";
echo $dll->pop()."\n";
echo $dll->pop()."\n";
echo $dll->pop()."\n";
echo $dll->pop()."\n";
echo $dll->pop()."\n";
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Exception: Offset invalid or out of range
Exception: Offset invalid or out of range
7
7
6
5
4
3
2
1
===DONE===
