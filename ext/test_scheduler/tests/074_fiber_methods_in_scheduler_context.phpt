--TEST--
test_scheduler: Fiber methods refuse to switch while the scheduler runs its own work
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
function attempt(string $name, callable $call): void
{
    try {
        $call();
        echo "$name: switched\n";
    } catch (FiberError $error) {
        echo "$name: ", $error->getMessage(), "\n";
    }
}

$suspended = new Fiber(function () {
    Fiber::suspend();
    echo "unreachable\n";
});
$suspended->start();

// Destroyed when the scheduler releases the finished coroutine, so the
// destructor runs in the scheduler context.
class Probe {
    public function __destruct()
    {
        global $suspended;

        attempt("start", function () {
            (new Fiber(function () {
                echo "unreachable\n";
            }))->start();
        });
        attempt("resume", fn () => $suspended->resume());
        attempt("throw", fn () => $suspended->throw(new Exception("unreachable")));
        attempt("suspend", fn () => Fiber::suspend());
        var_dump($suspended->isSuspended());
    }
}

TestScheduler\spawn(function () {
    return new Probe;
});

echo "main done\n";
?>
--EXPECT--
main done
start: Cannot switch fibers in current execution context
resume: Cannot switch fibers in current execution context
throw: Cannot switch fibers in current execution context
suspend: Cannot suspend outside of a fiber
bool(true)
