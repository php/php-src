--TEST--
GH-24286 (Tracing JIT preserves polymorphic call results and exceptions after Closure::__invoke)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_loop=61
opcache.jit_hot_func=127
opcache.jit_hot_return=8
opcache.jit_hot_side_exit=8
--FILE--
<?php
class Value {
    public $value;
    function get() { return $this->value; }
    function fail() { throw new RuntimeException('expected'); }
    function __call($method, $args) { return $this->value; }
}
function call($object, $method) {
    return $object->{$method}();
}
function every($items, $method) {
    foreach ($items as $item) {
        if (!call($item, $method)) return false;
    }
    return true;
}
$closure = function () { return false; };
for ($i = 0; $i < 2000; $i++) every([$closure], '__invoke');
$object = new Value;
foreach ([null, false, true, 42, 1.5, str_repeat('x', 3), [42], (object) ['n' => 42]] as $value) {
    $object->value = $value;
    for ($i = 0; $i < 100; $i++) {
        if (call($object, 'get') !== $value) {
            throw new RuntimeException('Incorrect result');
        }
    }
}
$object->value = 'magic';
var_dump(call($object, 'missing'));
var_dump(call($closure, '__invoke'));
try {
    call($object, 'fail');
} catch (RuntimeException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo "Done\n";
?>
--EXPECT--
string(5) "magic"
bool(false)
RuntimeException: expected
Done
