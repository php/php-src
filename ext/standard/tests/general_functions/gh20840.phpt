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

$buffer = '';
ob_start(function ($chunk) use (&$buffer) {
    $buffer .= $chunk;
    $buffer = preg_replace('(\s*object\(Node\)#\d+ \(\d+\) \{\s*)', '', $buffer);
    $buffer = preg_replace('(\s*\["next"\]=>\s*)', '', $buffer);
    $buffer = preg_replace('(\s*\}\s*)', '', $buffer);
});
var_dump($firstNode);
ob_end_flush();
echo $buffer;

while ($next = $firstNode->next) {
    $firstNode->next = $next->next;
}
?>
--EXPECT--
nesting level too deep
