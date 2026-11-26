--TEST--
GH-24605: unserialize() checks native stack limit on deep nesting
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
function test_unserialize(string $name, string $payload, array $options = []): void {
    try {
        $result = unserialize($payload, $options);
        echo $name, ': ', $result === false ? "false without error\n" : "NOT GUARDED\n";
    } catch (Error $e) {
        echo $name, ': ', str_contains($e->getMessage(), 'Maximum call stack size')
            ? "guarded\n"
            : "unexpected error\n";
    }
}

$depth = 20000;
$array = str_repeat('a:1:{i:0;', $depth) . 'i:0;' . str_repeat('}', $depth);
test_unserialize('array', $array, ['max_depth' => 0]);

$object = str_repeat('O:8:"stdClass":1:{s:1:"a";', $depth) . 'i:0;' . str_repeat('}', $depth);
test_unserialize('object', $object, ['max_depth' => 0]);
?>
--EXPECT--
array: guarded
object: guarded
