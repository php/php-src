--TEST--
RecursiveCallbackFilterIterator with trampoline callback
--FILE--
<?php

function test($value, $key, $inner) {
    if ($inner->hasChildren()) {
        return true;
    }
    printf("%s / %s / %d / %d\n"
        , print_r($value, true)
        , $key
        , $value == $inner->current()
        , $key == $inner->key()
    );
    return $value === 1 || $value === 4;
}

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        return test(...$arguments);
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

$it = new RecursiveArrayIterator([1, [2, 3], [4, 5]]);
$it = new RecursiveCallbackFilterIterator($it, $callback);
$it = new RecursiveIteratorIterator($it);

foreach($it as $value) {
    echo "=> $value\n";
}
?>
--EXPECT--
Trampoline for trampoline
1 / 0 / 1 / 1
=> 1
Trampoline for trampoline
Trampoline for trampoline
2 / 0 / 1 / 1
Trampoline for trampoline
3 / 1 / 1 / 1
Trampoline for trampoline
Trampoline for trampoline
4 / 0 / 1 / 1
=> 4
Trampoline for trampoline
5 / 1 / 1 / 1
