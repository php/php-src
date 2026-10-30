--TEST--
GH-24605 (var_export(), print_r() and unserialize() segfault on deep nesting)
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
$a = 0;
for ($i = 0; $i < 3000; $i++) {
    $a = [$a];
}
$u = str_repeat('a:1:{i:0;', 3000) . 'i:0;' . str_repeat('}', 3000);

$calls = [
    'var_export' => static fn() => var_export($a),
    'print_r' => static fn() => print_r($a, true),
    'unserialize' => static fn() => unserialize($u),
];

foreach ($calls as $name => $call) {
    try {
        $call();
        echo $name, ": no error\n";
    } catch (\Error $e) {
        echo $name, ': ',
            str_contains($e->getMessage(), 'Maximum call stack size') ? 'stack error' : 'other error',
            "\n";
    }
}
?>
--EXPECT--
var_export: stack error
print_r: stack error
unserialize: stack error