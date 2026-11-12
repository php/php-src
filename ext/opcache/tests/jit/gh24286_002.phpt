--TEST--
GH-24286 (Tracing JIT preserves unpacked dynamic call results during recursion)
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
class Box {
    function run(float $value): mixed { return $value; }
}
function invokeCall($object, $method, ...$args) {
    return $object->{$method}(...$args);
}
function numberText(mixed $value): string {
    if (!is_int($value) && !is_float($value)) return '';
    return (string) $value;
}
function walk($items, $method, $value, int $depth): string {
    $text = '';
    foreach ($items as $item) {
        $result = invokeCall($item, $method, $value);
        $text .= numberText($result);
        if ($result === false) return '';
        if ($depth > 0) $text .= walk([$item], $method, $value + 0.5, $depth - 1);
    }
    return $text;
}
$closure = function (float $value): mixed { return false; };
for ($i = 0; $i < 2000; $i++) walk([$closure], '__invoke', 0.5, 1);
for ($i = 0; $i < 500; $i++) {
    walk([new Box()], 'run', -184013402102942.84 + $i, 2);
}
echo walk([new Box()], 'run', 15.0, 2), "\n";
var_dump(invokeCall(new Box(), 'run', value: 15.5));
?>
--EXPECT--
1515.516
float(15.5)
