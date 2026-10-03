--TEST--
An EH_THROW error handling window stays with the fiber that opened it
--FILE--
<?php
// SplFileObject::__construct() opens an EH_THROW window (warnings become
// RuntimeException) and calls the user wrapper inside it.
class SuspendingWrapper {
    public $context;

    public function url_stat(string $path, int $flags): array
    {
        return ['mode' => 0100644];
    }

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool
    {
        $started = new Fiber(function () {
            trigger_error("in a fiber started inside the window", E_USER_WARNING);
        });
        $started->start();

        Fiber::suspend();

        try {
            trigger_error("inside the window after resume", E_USER_WARNING);
        } catch (RuntimeException $exception) {
            echo "caught: ", $exception->getMessage(), "\n";
        }

        return false;
    }
}

stream_wrapper_register('suspending', SuspendingWrapper::class);

$fiber = new Fiber(function () {
    try {
        new SplFileObject('suspending://file');
    } catch (RuntimeException $exception) {
        echo "constructor: ", get_class($exception), "\n";
    }
});

$fiber->start();
trigger_error("in main while the fiber is inside the window", E_USER_WARNING);
$fiber->resume();
trigger_error("in main after the window closed", E_USER_WARNING);
?>
--EXPECTF--
Warning: in a fiber started inside the window in %s on line %d

Warning: in main while the fiber is inside the window in %s on line %d
caught: inside the window after resume
constructor: RuntimeException

Warning: in main after the window closed in %s on line %d
