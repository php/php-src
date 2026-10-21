--TEST--
VM interrupt pending at a forward jump onto the op that frees a temporary
--DESCRIPTION--
A jump whose target frees or consumes a temporary that is live across the
jump: the target is the end of that temporary's live range, so an exception
thrown there by an interrupt must not leak the temporary.
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

// Not constant, so that the optimizer keeps the branches, and not interned
$n = 3;
function value() {
    return str_repeat('a', $GLOBALS['n']);
}

// The interrupt is still pending: the loop's backward jump throws it.
function after() {
    for ($i = 0; $i < 3; $i++) {
        echo "after() ", $i, "\n";
    }
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

test('ternary', function () {
    $c = value();
    $x = $c ? $c . release() : 'c';
});

test('ternary, array element', function () {
    $c = value();
    $a = [];
    $a['k'] = $c ? $c . release() : 'c';
});

// Interrupts are checked on return from a function in the CALL VM
test('interpolated last', function () {
    $f = 'release';
    $x = "<{$f()}";
});

test('match', function () {
    $c = value();
    $x = match ($c) {
        'aaa' => $c . release(),
        default => 'c',
    };
});

test('switch', function () {
    switch (value()) {
        case 'aaa':
            release();
            break;
        default:
            echo "default\n";
    }
});
?>
--EXPECT--
ternary:
release()
Exception: VM interrupt
ternary, array element:
release()
Exception: VM interrupt
interpolated last:
release()
Exception: VM interrupt
match:
release()
Exception: VM interrupt
switch:
release()
Exception: VM interrupt
