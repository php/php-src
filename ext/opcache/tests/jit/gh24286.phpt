--TEST--
GH-24286 (Tracing JIT preserves dynamic call results after Closure::__invoke)
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
class A {
    function m() { return false; }
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
var_dump(call(new A, 'm'));
?>
--EXPECT--
bool(false)
