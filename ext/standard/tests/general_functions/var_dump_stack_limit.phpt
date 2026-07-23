--TEST--
var_dump() and debug_zval_dump() guard against native stack overflow on deep structures
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
zend.max_allowed_stack_size=256K
--FILE--
<?php
$a = [];
for ($i = 0; $i < 50000; $i++) { $a = [$a]; }

class Node { public $next; }
$firstNode = new Node();
$node = $firstNode;
for ($i = 0; $i < 50000; $i++) { $newNode = new Node(); $node->next = $newNode; $node = $newNode; }

function guarded(callable $fn): string {
    ob_start();
    $fn();
    return str_contains(ob_get_clean(), 'nesting level too deep') ? "guarded\n" : "NO GUARD\n";
}

echo 'var_dump array: ', guarded(fn() => var_dump($a));
echo 'debug_zval_dump array: ', guarded(fn() => debug_zval_dump($a));
echo 'debug_zval_dump object: ', guarded(fn() => debug_zval_dump($firstNode));

while (is_array($a) && isset($a[0])) { $a = $a[0]; }
while ($next = $firstNode->next) { $firstNode->next = $next->next; }
?>
--EXPECT--
var_dump array: guarded
debug_zval_dump array: guarded
debug_zval_dump object: guarded
