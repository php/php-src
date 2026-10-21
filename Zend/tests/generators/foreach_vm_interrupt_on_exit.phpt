--TEST--
VM interrupt throwing as foreach over a generator exits the loop
--DESCRIPTION--
When the generator finishes, FE_RESET and FE_FETCH jump forward to the loop's
FE_FREE. An interrupt that throws there must not leak the iterator.
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=0
zend_test.observer.observe_function_names=release
zend_test.observer.set_vm_interrupt_on_end=1
--FILE--
<?php
function release() {
    echo "release()\n";
}

// The interrupt is still pending: the loop's backward jump throws it.
function after() {
    for ($i = 0; $i < 3; $i++) {
        echo "after() ", $i, "\n";
    }
}

function gen() {
    yield 1;
    release();
}

function gen_empty() {
    release();
    return;
    yield;
}

function &gen_ref() {
    $v = 1;
    yield $v;
    release();
}

function test(string $name, Closure $fn) {
    echo "$name:\n";
    try {
        $fn();
        after();
    } catch (Exception $e) {
        echo get_class($e), ': ', $e->getMessage(), "\n";
    }
}

test('foreach', function () {
    foreach (gen() as $v) {
        echo "value: $v\n";
    }
});

test('foreach, empty', function () {
    foreach (gen_empty() as $v) {
        echo "value: $v\n";
    }
});

test('foreach by reference', function () {
    foreach (gen_ref() as &$v) {
        echo "value: $v\n";
    }
});
?>
--EXPECT--
foreach:
value: 1
release()
Exception: VM interrupt
foreach, empty:
release()
Exception: VM interrupt
foreach by reference:
value: 1
release()
Exception: VM interrupt
