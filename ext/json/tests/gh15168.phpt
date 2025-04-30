--TEST--
GH-15168 (stack overflow in json_encode())
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

var_dump(json_encode($firstNode, depth: 500000));
var_dump(json_last_error());
var_dump(json_last_error_msg());

while ($next = $firstNode->next) {
    $firstNode->next = $next->next;
}

?>
--EXPECT--
bool(false)
int(1)
string(28) "Maximum stack depth exceeded"
