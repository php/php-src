--TEST--
GH-15169 (stack overflow when var serialization in ext/standard/var)
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

try {
    serialize($firstNode);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

while ($next = $firstNode->next) {
    $firstNode->next = $next->next;
}

?>
--EXPECT--
Maximum call stack size reached. Infinite recursion?
