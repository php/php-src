--TEST--
VM interrupt throwing at the end of a generator's finally block after "return" in the try block
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=0
zend_test.observer.observe_function_names=release
zend_test.observer.set_vm_interrupt_on_end=1
--FILE--
<?php
$runs = 0;

function release() {
    global $runs;
    $runs++;
}

function gen() {
    try {
        yield 1;
        return str_repeat('a', 3);
    } finally {
        release();
    }
}

$gen = gen();
try {
    echo "yielded ", $gen->current(), "\n";
    $gen->next();
    echo "gen() returned ", $gen->getReturn(), "\n";
} catch (Exception $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
var_dump($gen->valid());
echo "finally ran $runs time(s)\n";
?>
--EXPECT--
yielded 1
Exception: VM interrupt
bool(false)
finally ran 1 time(s)
