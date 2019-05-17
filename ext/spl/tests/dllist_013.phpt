--TEST--
SPL: DoublyLinkedList: insert operations
--FILE--
<?php
$dll = new SplDoublyLinkedList();
// errors
try {
	$dll->add(2,5);
} catch (OutOfRangeException $e) {
	echo "Exception: ".$e->getMessage()."\n";
}

$dll->add(0,6);						//	6
$dll->add(0,3);						//	3 6
// Insert in the middle of the DLL
$dll->add(1,4);						//	3 4 6
$dll->add(2,5);						//	3 4 5 6
$dll->unshift(2);					//	2 3 5 4 6
// Insert at the beginning and end of the DLL
$dll->add(0,1);						//	1 2 3 4 5 6
$dll->add(6,7);						//	1 2 3 4 5 6 7

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
--EXPECT--
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
