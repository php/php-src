--TEST--
Io\Poll\SignalHandle: signals that cannot be blocked, fault signals and non-integers are refused
--EXTENSIONS--
pcntl
--FILE--
<?php
$cases = [[SIGKILL], [SIGSTOP], [SIGSEGV], [SIGBUS], [SIGFPE], [SIGILL], [0], [-1], [PHP_INT_MAX], ["15"], [1.0], [null], [SIGUSR1, "x"]];
foreach ($cases as $signals) {
    try {
        new Io\Poll\SignalHandle($signals);
        echo "accepted\n";
    } catch (Throwable $e) {
        echo get_class($e), ": ", preg_replace('/between 1 and \d+/', 'between 1 and N', $e->getMessage()), "\n";
    }
}
$h = new Io\Poll\SignalHandle([SIGUSR1, SIGUSR1]);
var_dump($h->getSignals() === [SIGUSR1]);
?>
--EXPECTF--
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) must not contain signal %d, which cannot be blocked
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) must not contain signal %d, which cannot be blocked
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) must not contain signal %d, which cannot be blocked
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) must not contain signal %d, which cannot be blocked
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) must not contain signal %d, which cannot be blocked
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) must not contain signal %d, which cannot be blocked
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) signals must be between 1 and N
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) signals must be between 1 and N
ValueError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) signals must be between 1 and N
TypeError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) signals must be of type int, string given
TypeError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) signals must be of type int, float given
TypeError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) signals must be of type int, null given
TypeError: Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) signals must be of type int, string given
bool(true)
