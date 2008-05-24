--TEST--
SPL: DoublyLinkedList: std operations
--FILE--
<?php
$dll = new SplDoublyLinkedList();
// errors
try {
    $dll->pop();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
try {
    $dll->shift();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

// data consistency
$a = 2;
$dll->push($a);
$a = 3;
$dll->push(&$a);
$a = 4;
echo $dll->pop()."\n";
echo $dll->pop()."\n";

$a = 2;
$dll->unshift($a);
$a = 3;
$dll->unshift(&$a);
$a = 4;
echo $dll->shift()."\n";
echo $dll->shift()."\n";

// peakable
$dll->push(1);
$dll->push(2);
echo $dll->top()."\n";
echo $dll->bottom()."\n";
$dll->pop();
$dll->pop();

// countable
$dll->push(NULL);
$dll->push(NULL);
echo count($dll)."\n";
echo $dll->count()."\n";
var_dump($dll->pop());
var_dump($dll->pop());

// clonable
$dll->push(2);
$dll_clone = clone $dll;
$dll_clone->pop();
echo count($dll)."\n";
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Deprecated: Call-time pass-by-reference has been deprecated in %s on line %d

Deprecated: Call-time pass-by-reference has been deprecated in %s on line %d
Exception: Can't pop from an empty datastructure
Exception: Can't shift from an empty datastructure
3
2
3
2
2
1
2
2
NULL
NULL
1
===DONE===
