--TEST--
GH-19909 (SEGV zend_mm_realloc_heap during object destruction)
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
class Node
{
    public $next;
}

$firstNode = new Node();
$node = $firstNode;
for ($i = 0; $i < 30000; $i++) {
    $newNode = new Node();
    $node->next = $newNode;
    $node = $newNode;
}

// the error occurs during destruction when the script ends
?>
--EXPECTF--
Fatal error: Maximum call stack size reached. Infinite recursion? in Unknown on line 0
