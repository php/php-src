--TEST--
SPL: iterator_apply() with a trampoline
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        var_dump($arguments);
        if ($name === 'trampolineThrow') {
            throw new Exception('boo');
        }
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];
$callbackThrow = [$o, 'trampolineThrow'];

$it = new RecursiveArrayIterator([1, 21, 22]);

try {
    $iterations = iterator_apply($it, $callback);
    var_dump($iterations);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    iterator_apply($it, $callbackThrow);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    iterator_apply($it, $callback, 'not an array');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Trampoline for trampoline
array(0) {
}
int(1)
Trampoline for trampolineThrow
array(0) {
}
Exception: boo
TypeError: iterator_apply(): Argument #3 ($args) must be of type ?array, string given
