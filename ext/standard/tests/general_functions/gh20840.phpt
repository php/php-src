--TEST--
GH-20840 (var_dump() crash with nested objects)
--CREDITS--
bendrissou
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
if (getenv('SKIP_ASAN')) {
    die('skip ASAN needs different stack limit setting due to more stack space usage');
}
?>
--INI--
zend.max_allowed_stack_size=512K
--FILE--
<?php
class Node {
    public $next;
}

$firstNode = new Node();
$node = $firstNode;

for ($i = 0; $i < 50000; $i++) {
    $newNode = new Node();
    $node->next = $newNode;
    $node = $newNode;
}

var_dump($firstNode);

while ($next = $firstNode->next) {
    $firstNode->next = $next->next;
}
?>
--EXPECTREGEX--
^object\(Node\)#\d+ \(\d+\).*(nesting level too deep|["\s}]*)$
