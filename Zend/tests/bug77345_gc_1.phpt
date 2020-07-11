--TEST--
Bug #77345 (Segmentation faults stack overflow in cyclic garbage collector) (Bug #77427)
--INI--
zend.enable_gc = 1
--FILE--
<?php

class Node
{
    /** @var Node */
    public $previous;
    /** @var Node */
    public $next;
}

var_dump(gc_enabled());
var_dump('start');

$firstNode = new Node();
$firstNode->previous = $firstNode;
$firstNode->next = $firstNode;

$circularDoublyLinkedList = $firstNode;

for ($i = 0; $i < 200000; $i++) {
    $currentNode = $circularDoublyLinkedList;
    $nextNode = $circularDoublyLinkedList->next;

    $newNode = new Node();

    $newNode->previous = $currentNode;
    $currentNode->next = $newNode;
    $newNode->next = $nextNode;
    $nextNode->previous = $newNode;

    $circularDoublyLinkedList = $nextNode;
}
var_dump('end');
--EXPECT--
bool(true)
string(5) "start"
string(3) "end"