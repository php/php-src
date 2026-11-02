--TEST--
VM interrupt throwing at the end of nested finally blocks after "return" in the try block
--DESCRIPTION--
The inner finally block's FAST_RET jumps to the outer FAST_CALL, which is inside
the inner try block. An interrupt that throws there must not run the inner
finally block again, but the outer finally block still runs.
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=0
zend_test.observer.observe_function_names=release
zend_test.observer.set_vm_interrupt_on_end=1
--FILE--
<?php
$runs = [];

function release($name) {
    global $runs;
    $runs[] = $name;
}

function test() {
    try {
        try {
            return str_repeat('a', 3);
        } finally {
            release('inner');
        }
    } finally {
        release('outer');
    }
}

try {
    $result = test();
    echo "test() returned $result\n";
} catch (Exception $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
    echo 'previous: ', get_class($e->getPrevious()), ': ', $e->getPrevious()->getMessage(), "\n";
}
echo implode(', ', $runs), "\n";
?>
--EXPECT--
Exception: VM interrupt
previous: Exception: VM interrupt
inner, outer
