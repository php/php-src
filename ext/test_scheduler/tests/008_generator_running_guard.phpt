--TEST--
test_scheduler: advancing a generator parked in another coroutine is an error
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend};

function gen(): Generator {
    echo "gen enter\n";
    suspend(); // park the coroutine inside the generator body
    yield 1;
}

$g = gen();

spawn(function () use ($g) {
    $g->current(); // enters the body, parks
    echo "not executed\n";
});

spawn(function () use ($g) {
    try {
        $g->next(); // the generator is mid-advance in another coroutine
    } catch (Error $e) {
        echo "Error: ", $e->getMessage(), "\n";
    }
});

echo "main done\n";
?>
--EXPECT--
main done
gen enter
Error: Cannot resume an already running generator
