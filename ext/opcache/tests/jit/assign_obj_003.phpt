--TEST--
JIT ASSIGN_OBJ: Assign undefined vatiable to property
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
class Node {
    public $previous;
    public $next;
}

function xxx() {
    $firstNode = new Node();
//    $firstNode->previous = $firstNode;
    $firstNode->next = $firstNode;
    $circularDoublyLinkedList = null;
    for ($i = 0; $i < 2; $i++) {
        $currentNode = $circularDoublyLinkedList;
        $nextNode = $circularDoublyLinkedList->next;
        $newNode->next = $undef1->next; // <- ???
        $newNode = new Node();
        $currentNode->undef2 = new Node();
        $circularDoublyLinkedList = $nextNode;
    }
}

try {
	@xxx();
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Exception: Attempt to assign property "next" on null
