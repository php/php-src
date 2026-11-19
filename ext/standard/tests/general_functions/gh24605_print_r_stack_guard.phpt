--TEST--
GH-24605: print_r() checks native stack limit for arrays and objects
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
function test_print_r(string $name, mixed $value): void {
    try {
        print_r($value, true);
        echo "$name (return): NOT GUARDED\n";
    } catch (Error $e) {
        echo $name, ' (return): ', str_contains($e->getMessage(), 'Maximum call stack size')
            ? "guarded\n"
            : "unexpected error\n";
    }

    ob_start();
    try {
        print_r($value);
        $out = ob_get_clean();
        echo "$name (output): NOT GUARDED\n";
    } catch (Error $e) {
        $out = ob_get_clean();
        echo $name, ' (output): ', str_contains($e->getMessage(), 'Maximum call stack size')
            ? "guarded" : "unexpected error",
            $out === '' ? ", no partial output\n" : ", partial output\n";
    }
}

$data = 0;
for ($i = 0; $i < 5000; $i++) {
    $data = [$data];
}
test_print_r('array', $data);

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
test_print_r('object', $first);

// Break the chain iteratively to avoid teardown recursion.
$node = $first;
while ($node !== null) {
    $next = $node->next;
    $node->next = null;
    $node = $next;
}
?>
--EXPECT--
array (return): guarded
array (output): guarded, no partial output
object (return): guarded
object (output): guarded, no partial output
