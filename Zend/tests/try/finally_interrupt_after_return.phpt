--TEST--
VM interrupt throwing at the end of a finally block after "return" in the try block
--DESCRIPTION--
"return" inside a try block runs the finally block with FAST_CALL, and FAST_RET
then jumps back to the RETURN, which is inside the try block. An interrupt that
throws as the finally block ends belongs to the finally block: it must not run
the finally block again, and it discards the pending return value.
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

function test() {
    try {
        return str_repeat('a', 3);
    } finally {
        release();
    }
}

try {
    $result = test();
    echo "test() returned $result\n";
} catch (Exception $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
echo "finally ran $runs time(s)\n";
?>
--EXPECT--
Exception: VM interrupt
finally ran 1 time(s)
