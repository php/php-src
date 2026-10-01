--TEST--
Observer: VM interrupt during tracing JIT user function call
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
opcache.jit_max_polymorphic_calls=0
zend_test.observer.enabled=1
zend_test.observer.show_output=0
zend_test.observer.observe_function_names=ZendTestJitInterrupt\external_target
zend_test.observer.set_vm_interrupt_on_begin=1
--SKIPIF--
<?php
if (ini_get('opcache.jit') === false) die('skip JIT not available');
?>
--FILE--
<?php
namespace ZendTestJitInterrupt;

// Keep the callee in a separate file so the caller uses
// INIT_NS_FCALL_BY_NAME/DO_FCALL_BY_NAME, not INIT_FCALL/DO_UCALL.
require __DIR__ . '/observer_jit_vm_interrupt.inc';

function drive_probe(int $n): int
{
    $sum = 0;
    for ($i = 0; $i < $n; $i++) {
        $sum += external_target($i);
    }
    return $sum;
}

$total = 0;
for ($round = 0; $round < 300; $round++) {
    $total += drive_probe(128);
}

echo "total={$total}\n";
?>
--EXPECT--
total=2438400
