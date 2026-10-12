--TEST--
test_scheduler: a GC run during the unwind of live temporaries leaves the ones already freed alone
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

function thrower(): int {
    throw new Exception('thrown');
}

function g(array $objects): void {
    // Three short of a full buffer: the unwind frees the two arrays below,
    // the first one before the second fills the buffer.
    $i = 0;
    while (gc_status()['roots'] < gc_status()['threshold'] - 3) {
        $copy = $objects[$i++];
        unset($copy);
    }

    $r = [$objects[$i]] + ([$objects[$i + 1]] + thrower());
}

try {
    g($objects);
} catch (Exception $e) {
    echo "caught ", $e->getMessage(), "\n";
}
?>
--EXPECT--
caught thrown
