--TEST--
GH-24430 (SEGV in zend_generator_dtor_storage when GC closes cyclic generators in a Fiber)
--FILE--
<?php
$chunks = [];
for ($i = 0; $i < 32; $i++) {
    $chunks[] = str_repeat('A', 4000);
}
unset($chunks);

function inner($id) {
    try {
        yield;
    } finally {
        if ($id === 3) {
            throw new RuntimeException('from finally');
        }
    }
}

function wrapper($gen) {
    try {
        yield from $gen;
    } catch (Exception $e) {
    } finally {
    }
}

(new Fiber(function () {
    $holders = [];
    for ($i = 0; $i < 6; $i++) {
        $generator = wrapper(inner($i));
        $generator->current();
        $holder = new stdClass();
        $holder->generator = $generator;
        $holder->self = $holder;
        $holders[] = $holder;
    }
    unset($holders);
    try {
        gc_collect_cycles();
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }
}))->start();

echo "Done\n";
?>
--EXPECT--
from finally
Done
