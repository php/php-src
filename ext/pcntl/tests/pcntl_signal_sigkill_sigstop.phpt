--TEST--
pcntl_signal() returns false for SIGKILL and SIGSTOP
--EXTENSIONS--
pcntl
--FILE--
<?php
foreach ([SIGKILL, SIGSTOP] as $signo) {
    var_dump(pcntl_signal($signo, SIG_IGN));
    var_dump(pcntl_signal($signo, SIG_DFL));
    var_dump(pcntl_signal($signo, function () {}));
    var_dump(pcntl_get_last_error() === PCNTL_EINVAL);
}

foreach ([SIGKILL, SIGSTOP] as $signo) {
    foreach (['not_a_function', 42] as $handler) {
        try {
            var_dump(pcntl_signal($signo, $handler));
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}
echo "alive\n";
?>
--EXPECTF--
Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)
bool(true)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)
bool(true)
TypeError: pcntl_signal(): Argument #2 ($handler) must be of type callable|int, string given
ValueError: pcntl_signal(): Argument #2 ($handler) must be either SIG_DFL or SIG_IGN when an integer value is given
TypeError: pcntl_signal(): Argument #2 ($handler) must be of type callable|int, string given
ValueError: pcntl_signal(): Argument #2 ($handler) must be either SIG_DFL or SIG_IGN when an integer value is given
alive
