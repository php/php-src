--TEST--
GH-15496: Session handler ob_start crash
--FILE--
<?php
class MySessionHandler implements SessionHandlerInterface
{
    public function open ($save_path, $session_name): bool
    {
        return true;
    }

    public function close(): bool
    {
      return true;
    }

    public function read($id): string
    {
        return '';
    }

    public function write($id, $sess_data): bool
    {
        ob_start(function () {});

        return true;
    }

    public function destroy($id): bool
    {
        return true;
    }

    public function gc($maxlifetime): int
    {
        return 0;
    }
}

session_set_save_handler(new MySessionHandler());
session_start();

ob_start(function() {
    var_dump($b);
});

while (1) {
    $a[] = 1;
}
?>
--EXPECTF--
Fatal error: {closure%A}(): Cannot use output buffering in output buffering display handlers in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line %d
