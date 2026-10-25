--TEST--
GH-24605: var_export() checks native stack limit for arrays and objects
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
if (getenv('SKIP_ASAN')) {
    die('skip ASAN needs different stack limit setting');
}
?>
--INI--
zend.max_allowed_stack_size=256K
--FILE--
<?php
function test_export(string $name, mixed $value): void {
    try {
        var_export($value, true);
        echo "$name: NOT GUARDED\n";
    } catch (Error $e) {
        echo $name, ': ', str_contains($e->getMessage(), 'Maximum call stack size')
            ? "guarded\n"
            : "unexpected error\n";
    }
}

$data = 0;
for ($i = 0; $i < 5000; $i++) {
    $data = [$data];
}
test_export('array', $data);

// Detach the nested arrays without recursively destroying them.
while (is_array($data)) {
    $data = $data[0];
}

class StackNode { public ?StackNode $next = null; }
$first = new StackNode();
$node = $first;
for ($i = 0; $i < 5000; $i++) {
    $node->next = new StackNode();
    $node = $node->next;
}
test_export('object', $first);

// Break the chain iteratively to avoid teardown recursion.
$node = $first;
while ($node !== null) {
    $next = $node->next;
    $node->next = null;
    $node = $next;
}
?>
--EXPECT--
array: guarded
object: guarded
